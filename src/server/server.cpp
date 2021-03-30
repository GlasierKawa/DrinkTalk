#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "../utility/locker.h"
#include "threadpool.h"
#include "conn.h"
#include "../database/ChatDataBase.h"
#include "../database/DbPool.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd(int epollfd, int fd, bool one_shot);
extern int removefd(int epollfd, int fd);

//预先为每个可能的客户连接分配一个conn对象
conn* conns = new conn[MAX_FD];
DbPool *db_pool = DbPool::getInstance();

void addsig(int sig, void(handler)(int), bool restart = true) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void show_error(int connfd, const char *info) {
    printf("%s", info);
    send(connfd, info, strlen(info), 0);
    close(connfd);
}

int main (int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: %s port_number\n", argv[0]);
        return 1;
    }

    // const char *ip = argv[1];
    int port = atoi(argv[1]); //端口号

    //忽略SIGPIPE信号
    addsig(SIGPIPE, SIG_IGN);

    //创建线程池
    threadpool<conn>* pool = NULL;
    try {
        pool = new threadpool<conn>;
    }
    catch (...) {
        fprintf(stderr, "catch some exception when create threadpool.\n");
        return 1;
    }

    //void DbPool::init(string ip, string user, string password, string dbname, int port, int maxConn)
    db_pool->init("121.36.69.144", "root", "123456", "tengxun", 3306, 8);   //创建数据库连接池
    
    assert(conns);
    int user_count = 0;

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    struct linger tmp = {1, 0};
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &address.sin_addr);
    address.sin_port = htons(port);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret >= 0);

    ret = listen(listenfd, 5);
    assert(ret >= 0);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd, false);    //add listenfd to epollfd
    conn::m_epollfd = epollfd;

    while (true) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR) {
            printf("epoll failure\n");
            break;
        }

        //epoll 返回，遍历返回成功的事件列表，对每个事件分别判断
        for (int i = 0; i < number; i++) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {//有新用户建立连接
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr *)&client_address,
                                            &client_addrlength);
                if (connfd < 0) {
                    printf("errno is: %d\n", errno);
                    continue;
                }
                if (connfd >= MAX_FD) {
                    fprintf(stderr, "connfd >= MAX_FD\n");
                    continue;
                }
                if (conn::m_user_count >= MAX_FD) {
                    show_error(connfd, "Internal server busy");
                    continue;
                }
                conns[connfd].init(connfd, client_address);
                printf("Debug info: accept one connect, connection count = %d\n", conn::m_user_count);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                //如果有异常，直接关闭客户端连接
                conns[sockfd].close_conn();
                printf("Debug info: user count = %d\n", conn::m_user_count);                                   
            }
            else if (events[i].events & EPOLLIN) {
                //根据读的结果，决定将任务添加到线程池，还是关闭连接
                if (conns[sockfd].read()) {//读取客户端传送的数据
                    printf("read_buf:\n %s", conns[sockfd].m_read_buf);
                    pool->append(conns + sockfd);   //将客户端的请求加入请求队列，该操作会唤醒线程池中的一个线程为该客户端进行处理
                }
                else {
                    printf("Debug info: user count = %d\n", conn::m_user_count);
                    conns[sockfd].close_conn();             
                }
            }
            else if (events[i].events & EPOLLOUT) {
                //根据写的结果，决定是否关闭连接
                if (!conns[sockfd].process_write()) {
                    conns[sockfd].close_conn();
                    printf("Debug info: user count = %d\n", conn::m_user_count);
                }
            }
            else {
                fprintf(stderr, "epoll return from an unexpected event\n");
            }
        }
    }
    close(epollfd);
    close(listenfd);
    delete[] conns;
    delete pool;
    delete db_pool;
    return 0;
}