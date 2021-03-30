#include <cstring>
#include "json/json.h"
#include <iostream>
#include <string>
#include "../database/ChatDataBase.h"
#include "conn.h"
#include <unistd.h>
#include "../utility/common.h"
#include <vector>
#include <unordered_map>
#include "../database/DbPool.h"

using namespace std;

locker lock;
unordered_map<int, int> mp; //userid --> sockfd
extern conn* conns;         //连接数组
extern DbPool *db_pool;     //数据库连接池

//epoll文件描述符相关操作，开始~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*将文件描述符fd设置为非阻塞*/
int setnonblocking(int fd) {
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

/*将文件描述符fd注册到epollfd内核事件表中，并使用边沿触发模式*/
void addfd(int epollfd, int fd, bool one_shot) {
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP; //边沿触发模式，读取数据应该一次性读取完毕
	if (one_shot) {
        //EPOLLONESHOT事件：当一个线程在处理某个socket时，其他线程是不可能有机会操作该socket的
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd); //所有加入epoll监视的文件描述符都是非阻塞的
}

/*从epollfd标识的epoll内核事件表中删除fd上的所有注册事件*/
void removefd(int epollfd, int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

//对epollfd监视的文件描述符fd，添加ev事件
void modfd(int epollfd, int fd, int ev) {
	epoll_event event;
	event.data.fd = fd;
	event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}
//epoll文件描述符相关操作，结束~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//init
int conn::m_user_count = 0;
int conn::m_epollfd = -1;

//服务器和客户端建立连接时调用该函数，sockfd是该链接的服务器本地sock，addr是客户端的地址
void conn::init(int sockfd, const sockaddr_in& addr) {
    m_sockfd = sockfd;
	m_address = addr;
	m_userId = -1;

    //以下两行避免TIME_WAIT状态，仅用于调试，实际使用应去掉
	int reuse = 1;
	setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	addfd(m_epollfd, sockfd, true);
	m_user_count++;

	init();
}

//init all data member
void conn::init() {//长连接，调用该函数，初始化所有数据
    //初始化链接的所有成员
    memset(m_read_buf, '\0', sizeof m_read_buf);
    m_read_idx = 0;
    memset(m_write_buf, '\0', sizeof m_write_buf);
    m_write_idx = 0;
    cmd = "";
    reply = "";
    m_checked_idx = 0;
}


//关闭和客户端的连接
void conn::close_conn(bool real_close /*= true*/) {
    if (real_close && m_sockfd != -1) {
        removefd(m_epollfd, m_sockfd);  //从epoll的监视集合中删除连接
		m_sockfd = -1;
		m_user_count--;	//关闭一个连接，客户总数减一
	}
    //将用户状态修改为下线
    if (m_userId != -1) {
        ChatDataBase *db_conn;
        connectionRAII connRAII(db_conn, db_pool);//使用RAII技术从数据库连接池中获取一条数据库连接
        printf("Debug info: userId = %d offline\n", m_userId);
        lock.lock();
        db_conn->my_database_offline(m_userId);
        m_userId = -1;
        mp.erase(m_userId);
        lock.unlock();
    }
}


//循环读取客户数据，直到无数据可读或者对方关闭连接，注意epoll为边沿触发模式，一次读取数据要读到阻塞
//主线程调用，读取数据
bool conn::read() {
	if (m_read_idx >= READBUFSIZE) {
		return false;
	}

    //这里是否需要考虑将m_read_idx重新置零，不能重新将m_read_idx置零，一次收到的json可能不完整
	int bytes_read = 0;
	while (true) {
		bytes_read = recv(m_sockfd, m_read_buf + m_read_idx, READBUFSIZE - 
											m_read_idx, 0);
		if (bytes_read == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {//非阻塞io，读到EWOULDBLOCK说明数据已经读取完毕
				printf("Debug info: read done! EWOULDBLOCK.\n");
				break;
			}
			return false;//读取出现未知错误
		}
		else if (bytes_read == 0) {
			return false;
		}
		m_read_idx += bytes_read;
	}
	return true;
}


//process_read()中调用，处理用户登录的请求
void conn::do_login(const Json::Value &json) {
    int userid = json["userId"].asInt();
    string password = json["password"].asString();

    Json::Value ret;
    ret["cmd"] = "login-reply";
    
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);

    if ( db_conn->my_database_password_correct(userid, password) ) {//返回一个登录成功的提示信息，并将用户状态修改为在线
        ret["success"] = 1;
        
        //将用户状态设置为上线
        m_userId = userid;
        printf("login debug, m_userId = %d, m_sockfd = %d\n", m_userId, m_sockfd);
        lock.lock();
        mp[m_userId] = m_sockfd;   //上线加入map中
        lock.unlock();
    } else {
        ret["success"] = 0;
    }
    User u; //登录返回用户信息
    db_conn->my_database_user_information(u, userid);

    ret["user"] = userToJsonObj(u);
    reply = ret.toStyledString();
    /*
    if (reply.size() >= WRITEBUFSIZE) {
        fprintf(stderr, "error: write buffer overflow\n");
        return false;
    }
    */
    //copy to send buffer，这里有缓冲区溢出的风险，后期考虑加上判断
    memcpy(m_write_buf, reply.c_str(), reply.size());   
    m_write_buf[reply.size()] = '\0';
}


void conn::do_get_friend_list(const Json::Value &root) {
    int userid = root["userId"].asInt();
    
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    vector<User> users;
    vector<int> msgNum;
    db_conn->my_database_friend_list(users, msgNum, userid); //users是返回的列表
    Json::Value arr;
    for (int i = 0; i < users.size(); i++) {
        Json::Value temp = userToJsonObj(users[i]);
        temp["msgNum"] = msgNum[i];
        arr.append(temp);
    }
    
    Json::Value ret;
    ret["cmd"] = "getFriendList-reply";
    ret["user"] = arr; //如果arr为空，那么ret["user"] = null
    reply = ret.toStyledString();

    //将json数据拷贝到发送缓冲区中
    memcpy(m_write_buf, reply.c_str(), reply.size());   
    m_write_buf[reply.size()] = '\0';
}

void conn::do_register(const Json::Value &root) {
    string userName = root["userName"].asString();
    string password = root["password"].asString();
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    int userId;
    db_conn->my_database_user_password(userName, password, userId);
    
    Json::Value ret;
    ret["cmd"] = "register-reply";
    ret["userId"] = userId;
    reply = ret.toStyledString();

    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_insert_private_message(const Json::Value &root) {
    Message msg;
    msg.sendId = root["sendId"].asInt();
    msg.receiveId = root["receiveId"].asInt();
    msg.type = root["type"].asInt();
    msg.content = root["content"].asString();
    msg.time = root["time"].asString();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    db_conn->my_database_chat(msg); //插入消息记录
}

void conn::do_get_message_record(const Json::Value &root) {
    int userId1 = root["userId1"].asInt(); //sender 在线的人，发送该请求的用户
    int userId2 = root["userId2"].asInt(); //receiver

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    vector<Message> msg;
    db_conn->my_database_chat_search(msg, userId1, userId2);
    Json::Value arr;
    for (int i = 0; i < msg.size(); i++) {
        arr.append(messageToJsonObj(msg[i]));
    }
    Json::Value ret;
    ret["cmd"] = "getMessageRecord-reply";
    ret["sendId"] = userId2;
    ret["message"] = arr;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_offline(const Json::Value &root) {
    int userId = root["userId"].asInt();
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    db_conn->my_database_offline(userId);

    Json::Value ret;
    ret["cmd"] = "NONE";
    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}


void conn::do_search_user_by_id(const Json::Value &root) {
    int userId = root["userId"].asInt();
    int sendId = root["sendId"].asInt();
    User u;
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    Json::Value ret;
    ret["cmd"] = "searchUserById-reply";
    if (!db_conn->my_database_user_id_exist(userId)) {//如果不存在
        ret["exist"] = false;
        reply = ret.toStyledString();
        memcpy(m_write_buf, reply.c_str(), reply.size());
        m_write_buf[reply.size()] = '\0';
        return;
    }
    ret["exist"] = true;
    db_conn->my_database_user_information(u, userId); //查找用户信息
    ret["user"] = userToJsonObj(u);
    
    bool already = db_conn->my_database_is_friend(userId, sendId);//判断是否是好友
    ret["already"] = already;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

int conn::isOnline(int userId, int &sock) {
    sock = -1;
    lock.lock();
    if (mp.count(userId)) {
        sock = mp[userId];
        printf("userId = %d is online, sock = %d\n", userId, sock);
        lock.unlock();
        return true;
    }
    lock.unlock();
    return false; //用户不在线
}

void conn::do_add_friend(const Json::Value &root) {
    int sendId = root["sendId"].asInt();
    int receiveId = root["receiveId"].asInt();
    
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    //如果接受者在线
    int sock = -1;
    if (isOnline(receiveId, sock)) {//isOnline(userId, sock)，如果用户在线返回true，sock为该用户与服务器的文件描述符，否则sock返回-1
        //向接受者发送通知
        Json::Value notify;
        notify["cmd"] = "addFriendNotification";
        User u;
        db_conn->my_database_user_information(u, sendId);//给接受者返回发送者的信息
        notify["user"] = userToJsonObj(u);

        conns[sock].reply = notify.toStyledString();
        memcpy(conns[sock].m_write_buf, conns[sock].reply.c_str(), conns[sock].reply.size());
        conns[sock].m_write_buf[conns[sock].reply.size()] = '\0';
        printf("add friend send to receiveer:\n %s", conns[sock].m_write_buf);
        modfd(m_epollfd, conns[sock].m_sockfd, EPOLLOUT);
    } else {//不在线，向数据库中插入一条记录
        db_conn->my_database_add_new_friend_notification(sendId, receiveId);
    }

    //向发送者回应消息
    Json::Value ret;
    ret["cmd"] = "addFriend-reply";
    ret["success"] = 1;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_add_friend_confirm(const Json::Value &root) {
    int sendId = root["sendId"].asInt();
    int receiveId = root["receiveId"].asInt();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    db_conn->my_data_base_add_friend(sendId, receiveId); //接受者点击了确定添加好友，将好友之间的关系添加到数据库中，这个函数需要修改

    //如果发送者在线，向发送者发送添加好友的Json
    int sock = -1;
    if (isOnline(sendId, sock)) {
        Json::Value temp;
        temp["cmd"] = "addUserToFriendList";
        User t;
        db_conn->my_database_user_information(t, receiveId);
        temp["user"] = userToJsonObj(t);

        
        conns[sock].reply = temp.toStyledString();
        memcpy(conns[sock].m_write_buf, conns[sock].reply.c_str(), conns[sock].reply.size());
        conns[sock].m_write_buf[conns[sock].reply.size()] = '\0';
        printf("Debug info: sender = %d is online, sock = %d send addFriendToList\n json:%s\n", sendId, sock, reply.c_str());
        modfd(m_epollfd, sock, EPOLLOUT);
    } //如果发送者不在线，上线时，直接获取好友列表即可

    //接受者一定在线，向接受者发送添加好友的Json
    Json::Value ret;
    ret["cmd"] = "addUserToFriendList";
    User u;
    db_conn->my_database_user_information(u, sendId);//向接受者发送发送者的用户信息
    ret["user"] = userToJsonObj(u);

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}


void conn::do_read_add_friend_notification(const Json::Value &root) {
    int userId = root["userId"].asInt();
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    vector<User> users;
    db_conn->get_all_add_friend_notification(userId, users);  //获取所有接受者为userId的好友申请信息，返回申请者的用户信息
    
    Json::Value ret, arr;
    ret["cmd"] = "readAddFriendNotification-reply";
    for (auto &u: users) {
        arr.append(userToJsonObj(u));
    }
    ret["user"] = arr;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_create_group(const Json::Value &root) {//ok，还未测试
    int userId = root["userId"].asInt();
    string groupName = root["groupName"].asString();

    int groupId = -1;
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    db_conn->my_database_add_new_group(groupName, userId, groupId); //这个函数还没有实现，已添加

    Json::Value ret;
    ret["cmd"] = "createGroup-reply";
    Group g(userId, groupId, groupName);
    ret["group"] = groupToJsonObj(g);

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}


void conn::do_search_group_by_id(const Json::Value &root) {
    int groupId = root["groupId"].asInt();
    int sendId = root["sendId"].asInt();
    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    Json::Value ret;
    ret["cmd"] = "searchGroupById-reply";
    if (!db_conn->my_database_group_exist(groupId)) {
        ret["exist"] = false;
        reply = ret.toStyledString();
        memcpy(m_write_buf, reply.c_str(), reply.size());
        m_write_buf[reply.size()] = '\0';
        return ;
    }
    ret["exist"] = true;

    Group group;
    db_conn->my_database_search_group(group, groupId);
    ret["group"] = groupToJsonObj(group);
    
    bool already = db_conn->my_database_in_group(sendId, groupId);
    ret["already"] = already;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_request_add_group(const Json::Value &root) {
    int userId = root["userId"].asInt();
    int groupId = root["groupId"].asInt();
    int ownerId = root["ownerId"].asInt();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    db_conn->my_database_user_add_group(groupId, userId); //将用户插入群关系表中
    
    Json::Value ret;
    ret["cmd"] = "addGroupToList";
    Group group;
    db_conn->my_database_search_group(group, groupId);  //获取新加入群的信息返回给用户
    ret["group"] = groupToJsonObj(group);

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

//上线时发送下面的json，获取所有群列表
void conn::do_get_group_list(const Json::Value &root) {
    int userId = root["userId"].asInt();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    vector<Group> groups;
    vector<int> msgNum;
    db_conn->my_database_get_group(groups, msgNum, userId);//这个函数还没有实现

    Json::Value ret, arr;
    ret["cmd"] = "getGroupList-reply";
    for (int i = 0; i < groups.size(); i++) {
        auto temp = groupToJsonObj(groups[i]);
        temp["msgNum"] = msgNum[i];
        arr.append(temp);
    }
    ret["group"] = arr;
    
    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_send_group_message(const Json::Value &root) {
    int sendId = root["sendId"].asInt();
    int groupId = root["groupId"].asInt();
    string content = root["content"].asString();
    string time = root["time"].asString();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);

    Message msg;
    msg.sendId = sendId;
    msg.receiveId = groupId;
    msg.content = content;
    msg.time = time;
    db_conn->my_database_group_msg_insert(msg);//在数据库的消息记录表中插入一条数据
    vector<User> user;
    db_conn->my_database_get_group_user(groupId, user);//通过群id查询群中所有成员的信息，后期可以修改为获得在线的群成员
    for (auto u: user) {
        int sock = -1;
        if (isOnline(u.userId, sock)) {
            Json::Value ret;
            ret["cmd"] = "receiveGroupMessage";
            ret["sendId"] = sendId;
            ret["groupId"] = groupId;
            ret["content"] = content;
            ret["time"] = time;

            conns[sock].reply = ret.toStyledString();
            memcpy(conns[sock].m_write_buf, conns[sock].reply.c_str(), conns[sock].reply.size());
            conns[sock].m_write_buf[conns[sock].reply.size()] = '\0';
            modfd(m_epollfd, sock, EPOLLOUT);//注册可写事件
        }
    }

    Json::Value ret;
    ret["cmd"] = "sendGroupMessage-reply";
    ret["success"] = 1;
    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

void conn::do_open_group(const Json::Value &root) {
    int groupId = root["groupId"].asInt();
    int userId = root["userId"].asInt();

    ChatDataBase *db_conn;
    connectionRAII connRAII(db_conn, db_pool);
    
    Json::Value ret, uarr, marr;
    ret["cmd"] = "openGroup-reply";
    ret["groupId"] = groupId;
    ret["userId"] = userId;
    
    vector<User> users;
    db_conn->my_database_get_group_user(groupId, users);
    for (auto user: users) {
        Json::Value temp;
        temp["userId"] = user.userId;
        temp["userName"] = user.userName;
        temp["iconStr"] = user.iconStr;
        uarr.append(temp);
    }
    ret["user"] = uarr;

    vector<Message> msgs;
    db_conn->my_database_get_group_chat_msg(groupId, userId, msgs);
    for (auto msg: msgs) {
        Json::Value temp;
        temp["sendId"] = msg.sendId;
        temp["content"] = msg.content;
        temp["time"] = msg.time;
        marr.append(temp);
    }
    ret["msg"] = marr;

    reply = ret.toStyledString();
    memcpy(m_write_buf, reply.c_str(), reply.size());
    m_write_buf[reply.size()] = '\0';
}

//处理读取到的用户请求，
PROCESS_CODE conn::process_read() {
    //首先判断是否读取到了完整的json
    int left_bracket_num = 0, right_bracket_num = 0, idx = 0, complete = 0;
    // while (idx < m_read_idx && m_read_buf[idx] != '{') idx++;//json 解析开始
    // if (idx >= m_read_idx) {
    //     return DONE;
    // }
    while (idx < m_read_idx && m_read_buf[idx]) {
        if (m_read_buf[idx] == '{') left_bracket_num++;
        else if (m_read_buf[idx] == '}') right_bracket_num++;
        idx++;
        if (left_bracket_num == right_bracket_num && right_bracket_num != 0) {//读取到了完整的json
            complete = 1;
            //m_read_buf[idx] = 0;
            break;
        }
    }
    // if (idx >= m_read_idx) {
    //     return DONE;
    // }
    if (!complete) {
        fprintf(stdout, "json data format is incomplete \n");
        return INCOMPLETE;
    }

    //读取到了完整的json，下面调用jsoncpp，读取json的内容
    Json::Reader reader;
	Json::Value root;

    int cnt = 0;
    if (!(cnt = reader.parse(m_read_buf, root))) {
        fprintf(stderr, "error, parse json error");
        return INCOMPLETE;  //ERROR
    }

    // printf("m_read_buf[idx] = %c, m_read_buf + idx = %s", m_read_buf[idx], m_read_buf + idx);
    // while (idx < READBUFSIZE && (m_read_buf[idx] == '\0' || m_read_buf[idx] == '\t' || m_read_buf[idx] == '\n') ) idx++;
    
    
    
    string cmd = root["cmd"].asString();
    if (cmd == "register") {
        printf("Debug info: deal with register\n");
        do_register(root);
        printf("Debug info: register reply json: %s", reply.c_str());
        //return REPLY;
    } else if (cmd == "login") {
        printf("Debug info: deal with login");
        do_login(root);
        printf("Debug info: login reply json:\n%s,", reply.c_str());
        //return REPLY;
    } else if (cmd == "getFriendList") {    //返回用户的好友列表，包含未读取的离线消息
        printf("Debug info: deal with getFriendList\n");
        do_get_friend_list(root);
        printf("Debug info: get friend list reply:\n%s,", reply.c_str());
        //return REPLY;
    } else if (cmd == "sendMessage") {  //发送消息
        printf("Debug info: deal with sendMessage\n");
        do_insert_private_message(root);
        
        //如果用户在线，发送消息(json)
        int receiveId = root["receiveId"].asInt();
        int sock = -1;
        if (isOnline(receiveId, sock)) {//在线，获得接受者的socket
            root["cmd"] = "receiveMessage";
            conns[sock].reply = root.toStyledString();
            memcpy(conns[sock].m_write_buf, conns[sock].reply.c_str(), conns[sock].reply.size());
            conns[sock].m_write_buf[conns[sock].reply.size()] = '\0';
            printf("reply json:\n", conns[sock].reply.c_str());
            modfd(m_epollfd, sock, EPOLLOUT);//注册可写事件
        }
        

        Json::Value temp;
        temp["cmd"] = "NONE";
        reply = temp.toStyledString();
        memcpy(m_write_buf, reply.c_str(), reply.size());
        m_write_buf[reply.size()] = '\0';
        printf("Debug info: sendMessage reply json:\n %s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "getMessageRecord") { //查找聊天记录
        printf("Debug info: deal with getMessageRecord\n");
        do_get_message_record(root);
        printf("Debug info: getMessageRecord reply json:\n%s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "offline") {
        printf("Debug info: deal with offline\n");
        do_offline(root);
        printf("Debug info: offline reply:\n %s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "searchUserById") { //搜索好友时，根据用户id查找获得一个用户的所有信息
        printf("Debug info: deal with searchUserById\n");
        do_search_user_by_id(root);
         printf("Debug info: searchUserById reply\n %s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "addFriend") {    //添加用户请求
        printf("Debug info: deal with addFriend\n");
        do_add_friend(root);
        printf("Debug info: addFriend reply\n %s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "addFriendConfirm") { 
        printf("Debug info: deal with addFriendConfirm\n");
        do_add_friend_confirm(root);
        printf("Debug info: add friend confirm reply\n %s\n", reply.c_str());
        //return REPLY;   //将发送者的信息返回给接受者
    } else if (cmd == "readAddFriendNotification") { //上线时发送该请求，读取所有离线的添加好友请求
        printf("Debug info: deal with readAddFriendNotification\n");
        do_read_add_friend_notification(root);
        printf("Debug info: readAddFriendNotification reply\n %s\n", reply.c_str());
        //return REPLY;
    } else if (cmd == "createGroup") {
        printf("Debug info: deal with createGroup\n");
        do_create_group(root);
        printf("Debug info: createGroup reply\n %s\n", reply.c_str());
    } else if (cmd == "searchGroupById") {
        printf("Debug info: deal with seachGroupById\n");
        do_search_group_by_id(root);
        printf("Debug info: seachGroupById reply\n %s\n", reply.c_str());
    } else if (cmd == "requestAddGroup") { //申请加入群
        printf("Debug info: deal with requestAddGroup\n");
        do_request_add_group(root);
        printf("Debug info: requestAddGroup reply\n %s\n", reply.c_str());
    } else if (cmd == "getGroupList") {
        printf("Debug info: deal with getGroupList\n");
        do_get_group_list(root);
        printf("Debug info: getGroupList reply\n %s\n", reply.c_str());
    } else if (cmd == "openGroup") {
        printf("Debug info: deal with openGroup\n");
        do_open_group(root);
        printf("Debug info: openGroup reply\n %s\n", reply.c_str());
    } else if (cmd == "sendGroupMessage") {
        printf("Debug info: deal with sendGroupMessage\n");
        do_send_group_message(root);
        printf("Debug info: sendGroupMessage reply\n %s\n", reply.c_str());
    }

    // while (idx < m_read_idx && m_read_buf[idx] != '{') idx++;//json 解析开始
    // if (idx < m_read_idx && m_read_buf[idx] == '{') { //可能m_read_buf里收到了多个json请求
    //     memcpy(m_read_buf, m_read_buf + idx, READBUFSIZE - idx);
    //     modfd(m_epollfd, m_sockfd, EPOLLIN);//添加可读事件，尝试再次读取json
    //     printf("Again %s\n", m_read_buf + idx);
    //     printf("m_read_buf = %s\n", m_read_buf);
    //     printf("idx = %c\n", m_read_buf[idx]);
    //     process_read();
    // }
    return REPLY;
}

//由线程池中的工作线程调用，这是处理客户请求的入口函数
void conn::process() {
	PROCESS_CODE read_ret = process_read();
	if (read_ret == INCOMPLETE) {
		modfd(m_epollfd, m_sockfd, EPOLLIN); //继续读取数据
		return;
	}
	else if (read_ret == REPLY) {//已经准备好要写回的json，存放在reply
        modfd(m_epollfd, m_sockfd, EPOLLOUT);	//对m_sockfd添加对可写事件，此时主线程epoll_wait返回，处理写事件
    } 
    // else {
    //     modfd(m_epollfd, m_sockfd, EPOLLIN);
    // }
}


//sock可写时，主线程epoll返回，会调用该函数，向客户端写入数据
bool conn::process_write() {
    int temp = 0;
	int bytes_have_send = m_write_idx;//数据如果没有一次性发完，接着发送数据
	int bytes_to_send = reply.size() - m_write_idx;//发送一个字符串结束符 '\0'
	if (bytes_to_send == 0) {
		modfd(m_epollfd, m_sockfd, EPOLLIN);
		init();
		return true;
	}
    
	while (1) {
		temp = write(m_sockfd, m_write_buf + bytes_have_send, bytes_to_send);
		if (temp <= -1) {
			if (errno == EAGAIN) {
				/*如果TCP写缓冲区没有空间，等待下一轮EPOLLOUT事件。虽然在此期间，
				服务器无法立即接收到同一客户的下一个请求，但这可以保证连接的完整性*/
				modfd(m_epollfd, m_sockfd, EPOLLOUT);
				return true;
			}
			fprintf(stderr, "write error\n");
			return false;
		}
		bytes_to_send -= temp;
		m_write_idx += temp;
		bytes_have_send = m_write_idx;
		if (bytes_have_send >= reply.size()) { //数据写完了
			//init(); //长连接，重新初始化连接的内容

            //--------------由于使用了epolloneshot事件，一定要加上下面这句话，让其他线程有机会处理该连接-------------
			modfd(m_epollfd, m_sockfd, EPOLLIN);    //重置socket上的EPOLLONESHOT事件，让其他线程有机会继续处理这个socket
            break;
		}
	}
    init(); //数据传输完毕，清空连接缓存
	return true;
}