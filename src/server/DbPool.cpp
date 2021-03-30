//
// Created by Administrator on 2021/3/19.
//

#include "../database/DbPool.h"


DbPool::DbPool() {
    m_curConn = 0;
    m_freeConn = 0;
}

void DbPool::DestroyPool() {
    list_mutex.lock();
    while(!connList.empty())
    {
        ChatDataBase *con = connList.front();
        connList.pop_front();
        delete con;
    }
    m_curConn = 0;
    m_freeConn = 0;
    connList.clear();
    list_mutex.unlock();
    cout<<"Destroy Database Connection pool."<<endl;
}

DbPool::~DbPool() {
    DestroyPool();
}

void DbPool::init(string ip, string user, string password, string dbname, int port, int maxConn) {
    m_ip = ip;
    m_user = user;
    m_password = password;
    m_databaseName = dbname;
    m_port = port;
    for(int i =0;i<maxConn;i++)
    {
        ChatDataBase *con = new ChatDataBase;
        con->initDb(ip,user,password,dbname);
        connList.push_back(con);
        ++m_freeConn;
    }
    m_maxConn = m_freeConn;
    list_sem = sem(m_freeConn);
    cout << "Create DataBase Connection pool, DataBase Connection num = " << m_maxConn<<endl;
}

ChatDataBase *DbPool::getConnection() {
    if(connList.empty())
        return nullptr;
    list_sem.wait();// 有连接来时候加1

    //多线程访问时
    list_mutex.lock();
    ChatDataBase *con = connList.front();
    connList.pop_front();
    m_freeConn--;
    m_curConn++;
    list_mutex.unlock();

    return con;
}

bool DbPool::releaseConnection(ChatDataBase *conn) {
    if(conn==NULL)
    {
        return false;
    }
    list_mutex.lock();
    connList.push_back(conn);
    ++m_freeConn;
    --m_curConn;
    list_mutex.unlock();
    list_sem.post();
    return true;
}

int DbPool::GetFreeConn() {
    return this->m_freeConn;
}

DbPool *DbPool::getInstance() {
    static DbPool connPool;
    return &connPool;
}


connectionRAII::connectionRAII(ChatDataBase *&con, DbPool *connPoll) {
    con = connPoll->getConnection();
    conRAII = con;
    pollRAII = connPoll;
}

connectionRAII::~connectionRAII() {
    pollRAII->releaseConnection(conRAII);
}
