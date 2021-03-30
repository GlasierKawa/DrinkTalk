//
// Created by Administrator on 2021/3/16.
//
#include <stdlib.h>
#include <vector>
#include "../database/ChatDataBase.h"
ChatDataBase::ChatDataBase()
{
    /*my_database_connect("tengxun");*/
    mysql = mysql_init(NULL);
    if(mysql == NULL)
    {
        cout << "Error:" << mysql_error(mysql);
        exit(1);
    }
}

ChatDataBase::~ChatDataBase()
{
    /*mysql_close(mysql);*/
    if(mysql != NULL)  //关闭数据连接
    {
        mysql_close(mysql);
    }
}

void ChatDataBase::my_database_connect(const char *name)
{
    mysql = mysql_init(NULL);
    mysql = mysql_real_connect(mysql, "121.36.69.144", "root", "123456", name, 3306, NULL, 0);\
    if (NULL == mysql)
    {
        cout << "connect database failure" << endl;
    } else{
        cout<<"connect database success"<<endl;
    }

    if (mysql_query(mysql, "set names utf8;") != 0)
    {
        cout << "mysql_query error" << endl;
    }
    // else
    // {
    //     cout << "set name utf8 success and mysql_query success" <<endl;
    // }

}

int ChatDataBase::my_database_get_group_name(string *s)
{
    if (mysql_query(mysql, "show tables;") != 0)
    {
        cout << "mysql_query error" << endl;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if (NULL == res)
    {
        cout << "mysql_store_result" << endl;
    }

    int count = 0;
    MYSQL_ROW row;
    while (row = mysql_fetch_row(res))
    {
        s[count] += row[0];
        count++;
    }

    return count;
}

void ChatDataBase::my_database_get_group_member(string name, string &s)
{
    char sql[1024] = {0};
    sprintf(sql, "select member from %s;", name.c_str());
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if (NULL == res)
    {
        cout << "mysql_store_result error" << endl;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (NULL == row[0])
    {
        return;
    }
    s += row[0];
}




void ChatDataBase::my_database_user_password(string name, string password,int &userid)
{
    int imgIdx = rand() % 9;
    char sql[128] = {0};
    char iconStr[100] = {0};
    sprintf(iconStr, ":/media/person/media/person/%d.jpg", imgIdx);
    sprintf(sql, "insert into T_USER (f_user_name,f_password,f_online,f_photo,f_signature) values('%s','%s','%d','%s','%s');", name.c_str(),password.c_str(),0,iconStr,"happy");
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "SELECT max(f_user_id) from T_USER ;");
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    userid = stoi(row[0]);
}

bool ChatDataBase::my_database_password_correct(int id, string password)
{
    char sql[256] = {0};
    sprintf(sql, "select f_password from T_USER where f_user_id='%d';",id);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && password == row[0])
    {
        memset(sql,0,sizeof(sql));
        sprintf(sql,"UPDATE T_USER SET f_online=1 where f_user_id='%d';",id);
        if (mysql_query(mysql, sql) != 0) {
            cout << "update user online information error" << endl;
            return false;
        }
        else
        {
            //cout<<"update user online information success" <<endl;
            return true;
        }
    }
    else
    {
        cout<<"user no exist or password false"<<endl;
        return false;
    }
}



bool ChatDataBase::my_database_is_friend(int n1, int n2)
{
    char sql[128] = {0};
    sprintf(sql, "SELECT * FROM T_USER_RELATIONSHIP WHERE T_USER_RELATIONSHIP.f_user_id1 = '%d' AND T_USER_RELATIONSHIP.f_user_id2 = '%d';", n1, n2);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    int  num_rows=mysql_num_rows(res); 
    if (num_rows) {  //已经是好友了
        return true;
    }
    else {
        return false;
    }
}

void ChatDataBase::my_database_add_new_friend_notification(int n1, int n2)
{
    if(my_database_is_friend(n1, n2))
    {
        cout<<"friend already"<<endl;
        return ;
    }
    char sql[1024] = {0};
    //n1 发送 好友请求  n2 接受
    sprintf(sql,"insert into T_FRIEND_NOTIFICATION (f_user_id1,f_user_id2) values('%d','%d')",n1,n2);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "insert notification error" << endl;
    } 
    // else{
    //     cout << "insert notification  success" << endl;
    // }
}

bool ChatDataBase::my_database_group_exist(int gid)
{
    char sql[128] = {0};
    sprintf(sql, "select * from T_GROUP WHERE f_group_id = '%d';", gid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    int  num_rows = mysql_num_rows(res); 
    if (num_rows == 0)//群不存在
    {
        return false;
    }
    else//群存在
    {
        return true;
    }
}

void ChatDataBase::my_database_friend_list(vector<User> &friendlist,vector<int> &friend_message, int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select f_user_id2,f_user_name,f_online,f_photo,f_signature,f_message_num from T_USER_RELATIONSHIP, T_USER where T_USER_RELATIONSHIP.f_user_id1 = '%d' and T_USER_RELATIONSHIP.f_user_id2 = T_USER.f_user_id", userid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return;
    }
    // else
    // {
    //     cout << "mysql_query success" <<endl;
    // }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    cout<<numField<<endl;
    /*for(int i=0;i<numField;i++)
    {
        cout<<mysql_fetch_field(res)->name<<endl;
    }*/
    while((row=mysql_fetch_row(res))!=NULL)
    {
        User temp;
        for(int i=0;i<numField;i++)
        {
            if(i==0)
                temp.userId=stoi(row[i]);
            if(i==1)
                temp.userName=row[i];
            if(i==2)
                temp.online=stoi(row[i]);
            if(i==3)
                temp.iconStr=row[i];
            if(i==4)
                temp.desc=row[i];
        }
        friendlist.push_back(temp);
        friend_message.push_back(stoi(row[5]));
    }
}

bool ChatDataBase::my_database_user_id_exist(int userid) {
    char sql[128] = {0};
    sprintf(sql, "select * from T_USER WHERE f_user_id = '%d';", userid);
    if (mysql_query(mysql, sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return false;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    int  num_rows = mysql_num_rows(res);
    if (num_rows) {
        cout << "my_database_user_id_exist return true" << endl;
        return true;
    }
    cout << "my_database_user_id_exist return false" << endl;
    return false;
}

bool ChatDataBase::my_database_group_id_exist(int groupid) {
    char sql[128] = {0};
    sprintf(sql, "select f_group_id from T_GROUP WHERE f_group_id = '%d';", groupid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return false;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    int  num_rows = mysql_num_rows(result);
    if (num_rows) {//该群存在
        return true;
    }
    return false;
}

void ChatDataBase::my_database_user_information(User &ur, int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select f_user_id,f_user_name,f_online,f_photo,f_signature from T_USER where f_user_id='%d';", userid);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        for(int i=0;i<numField;i++)
        {
            if(i==0)
                ur.userId=stoi(row[i]);
            if(i==1)
                ur.userName=row[i];
            if(i==2)
                ur.online=stoi(row[i]);
            if(i==3)
                ur.iconStr=row[i];
            if(i==4)
                ur.desc=row[i];
        }
    }
}

void ChatDataBase::my_database_chat(Message ms) {
    char sql[1024] = {0};
    sprintf(sql, "INSERT into T_CHAT_MESSAGE(f_senderid,f_targetid,f_msgcontent,f_sendtime,f_type) VALUES('%d','%d','%s','%s','%d');", ms.sendId,ms.receiveId,ms.content.c_str(),ms.time.c_str(),ms.type);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_insert error" << endl;
        return;
    }
    // else
    // {
    //     cout << "mysql_insert success" <<endl;
    // }
    bool temp = my_database_user_state(ms.receiveId);
    if(temp == false)
    {
        cout<<ms.receiveId<<"not online"<<endl;
        memset(sql,0,sizeof(sql));
        sprintf(sql,"update T_USER_RELATIONSHIP SET f_message_num=f_message_num+1 WHERE f_user_id1='%d' and f_user_id2='%d';",ms.receiveId,ms.sendId);
        if (mysql_query(mysql, sql) != 0)
        {
            cout<<"error"<<endl;
        }
    }

}

void ChatDataBase::my_database_chat_search(vector<Message> &messageList, int senderid, int targetid) {
    char sql[1024] = {0};
    sprintf(sql, "select f_senderid,f_targetid,f_msgcontent,f_sendtime,f_type from T_CHAT_MESSAGE where (f_senderid='%d' and f_targetid='%d')or(f_senderid='%d' and f_targetid='%d')", senderid,targetid,targetid,senderid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return;
    }
    // else
    // {
    //     cout << "mysql_query success" <<endl;
    // }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    Message temp;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        for(int i=0;i<numField;i++) {
            //cout << i<<":"<<row[i] << endl;
            if(i==0){temp.sendId=stoi(row[i]);}
            if(i==1){temp.receiveId=stoi(row[i]);}
            if(i==2){temp.content=row[i];}
            if(i==3){temp.time=row[i];}
            if(i==4){temp.type=stoi(row[i]);}
        }
        messageList.push_back(temp);
    }
    memset(sql,0,sizeof(sql));
    sprintf(sql,"update T_USER_RELATIONSHIP SET f_message_num=0 WHERE f_user_id1='%d' and f_user_id2='%d';", senderid,targetid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return;
    }
}

bool ChatDataBase::search_user(int userid, User &ur) {
    if(my_database_user_id_exist(userid))
    {
        my_database_user_information(ur,userid);
        return true;
    }
    else{
        return false;
    }
}

void ChatDataBase::my_database_add_new_friend(int sender_id, int receive_id) {
    char sql[1024] = {0};
    memset(sql, 0, sizeof(sql));
    sprintf(sql,"select f_user_id1,f_user_id2 from T_FRIEND_NOTIFICATION where f_user_id1='%d' and f_user_id2='%d';",sender_id,receive_id);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "insert friend relationship information error" << endl;
    }
    // else
    // {
    //     cout << "insert friend relationship information success" << endl;
    // }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row!=NULL){
        memset(sql, 0, sizeof(sql));
        sprintf(sql,"insert T_USER_RELATIONSHIP(f_user_id1,f_user_id2)values('%d','%d');",sender_id,receive_id);
        if(mysql_query(mysql,sql)!=0){
            cout<<"insert error"<<endl;
        }
        memset(sql, 0, sizeof(sql));
        sprintf(sql,"insert T_USER_RELATIONSHIP(f_user_id1,f_user_id2)values('%d','%d');",receive_id,sender_id);
        if(mysql_query(mysql,sql)!=0){
            cout<<"insert error"<<endl;
        }
        memset(sql, 0, sizeof(sql));
        sprintf(sql,"delete from T_FRIEND_NOTIFICATION where f_user_id1='%d' and f_user_id2='%d';",sender_id,receive_id);
        if(mysql_query(mysql,sql)!=0){
            cout<<"delete error"<<endl;
        }
    }
}

void ChatDataBase::my_database_search_notification(int userid, User &ur) {
    char sql[1024] = {0};
    memset(sql, 0, sizeof(sql));
    sprintf(sql,"select f_user_id2 FROM T_FRIEND_NOTIFICATION where f_user_id1='%d';",userid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "select error" << endl;
    }
    // else
    // {
    //     cout << "select success" << endl;
    // }
    auto res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    //cout<<row[0]<<endl;
    auto temp = row[0];
    int userid2 = stoi(temp);
    search_user(userid2,ur);
}

void ChatDataBase::my_database_offline(int userid) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"UPDATE T_USER SET f_online=0 where f_user_id='%d';",userid);
    if (mysql_query(mysql, sql) != 0) {
        cout << "update user offline information error" << endl;
        return ;
    }
    // else
    // {
    //     cout<<"update user offline information success" <<endl;
    // }
}

void ChatDataBase::my_database_search_group_notification(int groupownerid, vector<User> &userList) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"SELECT f_user_id from T_GROUP_NOTIFICATION where f_type='add' and f_group_owner_id='%d';",groupownerid);
    if(mysql_query(mysql,sql) != 0)
    {
        cout<<"get users of group failed"<<endl;
    }
    // else{
    //     cout<<"get users of group success"<<endl;
    // }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    User temp;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        cout<<row[0]<<endl;
        my_database_user_information(temp,stoi(row[0]));
        userList.push_back(temp);
    }
}

void ChatDataBase::my_database_group_owner_add_user(int groupid, int userid) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"insert into T_GROUP_MEMBER(f_group_id,f_user_id) values('%d','%d')",groupid,userid);
    if(mysql_query(mysql,sql) !=0)
    {
        cout<<"insert failed"<<endl;
    }
    // else{
    //     cout<<"insert success"<<endl;
    // }
}

void ChatDataBase::my_database_delete_friend(int userid, int friendid) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"delete from T_USER_RELATIONSHIP where f_user_id1='%d' and f_user_id2='%d';",userid,friendid);
    if(mysql_query(mysql,sql)!=0)
    {
        cout<<"query error"<<endl;
        return ;
    }
    // else{
    //     cout<<"query success" <<endl;
    // }
    memset(sql,0,sizeof(sql));
    sprintf(sql,"delete from T_USER_RELATIONSHIP where f_user_id1='%d' and f_user_id2='%d';",friendid,userid);
    if(mysql_query(mysql,sql)!=0)
    {
        cout<<"query error"<<endl;
        return ;
    }
    // else{
    //     cout<<"query success" <<endl;
    // }
}

bool ChatDataBase::my_database_user_state(int userid) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"select T_USER.f_online from T_USER where f_user_id='%d';",userid);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return false; //抛出异常还是 返回false
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(stoi(row[0])==1)
        return true;
    else{
        return false;
    }
}

bool ChatDataBase::initDb(string host, string user, string passwd, string db_name) {
    mysql = mysql_real_connect(mysql, host.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), 3306, NULL, 0);
    if(mysql == NULL)
    {
        cout << "Error: " << mysql_error(mysql);
        exit(1);
    }
    if (mysql_query(mysql, "set names utf8;") != 0)
    {
        cout << "mysql_query error" << endl;
    }
    return true;
}

bool ChatDataBase::exeSQL(string sql) {
    if (mysql_query(mysql,sql.c_str()))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return false;
    }
    else // 查询成功
    {
        result = mysql_store_result(mysql);  //获取结果集
        if (result)  // 返回了结果集
        {
            int  num_fields = mysql_num_fields(result);   //获取结果集中总共的字段数，即列数
            int  num_rows=mysql_num_rows(result);       //获取结果集中总共的行数
            for(int i=0;i<num_rows;i++) //输出每一行
            {
                //获取下一行数据
                row=mysql_fetch_row(result);
                if(row<0) break;

                for(int j=0;j<num_fields;j++)  //输出每一字段
                {
                    cout<<row[j]<<"\t\t";
                }
                cout<<endl;
            }

        }
        else  // result==NULL
        {
            if(mysql_field_count(mysql) == 0)   //代表执行的是update,insert,delete类的非查询语句
            {
                // (it was not a SELECT)
                int num_rows = mysql_affected_rows(mysql);  //返回update,insert,delete影响的行数
                printf("%s execute successed! affect %d rows\n", sql.c_str(), num_rows);
            }
            else // error
            {
                cout<<"Get result error: "<<mysql_error(mysql);
                return false;
            }
        }
    }
    return true;
}

void ChatDataBase::get_all_add_friend_notification(int userId, vector<User> &users) {
    char sql[1024] = {0};
    mysql_query(mysql, "start transaction");
    sprintf(sql, "SELECT T_USER.f_user_id, T_USER.f_user_name, T_USER.f_online, \
            T_USER.f_photo, T_USER.f_signature from T_FRIEND_NOTIFICATION, T_USER WHERE \
            T_FRIEND_NOTIFICATION.f_user_id1 = T_USER.f_user_id AND T_FRIEND_NOTIFICATION.f_user_id2 = '%d';", userId);
    if (mysql_query(mysql, sql)) {
        fprintf(stderr, "select add friend notification failed\n");
        mysql_query(mysql,"rollback");
        return;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row;
    auto numField = mysql_num_fields(res);
    while(row = mysql_fetch_row(res))
    {
        User t;
        for (int i = 0; i < numField; i++) {
            if (i == 0) {
                t.userId = stoi(row[i]);
            } else if (i == 1) {
                t.userName = row[i];
            } else if (i == 2) {
                t.online = stoi(row[i]);
            } else if (i == 3) {
                t.iconStr = row[i];
            } else if (i == 4) {
                t.desc = row[i];
            }
        }
        users.push_back(t);
    }
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "DELETE FROM T_FRIEND_NOTIFICATION WHERE T_FRIEND_NOTIFICATION.f_user_id2 = '%d';", userId);
    if (mysql_query(mysql, sql)) {
        fprintf(stderr, "delete friend notification failed\n");
        mysql_query(mysql,"rollback");
        return;
    }
    if(mysql_query(mysql,"commit"))
    {
        fprintf(stderr,"mysql commit failure.\n");
    }
    return;
}

void ChatDataBase::my_data_base_add_friend(int sendId, int receiveId) {
    char sql[256] = {0};
    sprintf(sql, "INSERT INTO T_USER_RELATIONSHIP VALUES ('%d', '%d', '0');", sendId, receiveId);
    if (mysql_query(mysql, sql)) {
        cout << "insert friend relationship error" << endl;
        return ;
    }
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "INSERT INTO T_USER_RELATIONSHIP VALUES ('%d', '%d', '0');", receiveId, sendId);
    if (mysql_query(mysql, sql)) {
        cout << "insert friend relationship error" << endl;
        return ;
    }
}

void ChatDataBase::my_database_user_add_group_notification(int user_id,int group_id)
{
    if(!my_database_group_id_exist(group_id)) return;
    char sql[1024] = {0};
    sprintf(sql, "select f_group_owner_id from T_GROUP where f_group_id='%d';", group_id);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }
    else
    {
        cout << "mysql_query success" << endl;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    string temp_group_owner_id = row[0];
    cout<<temp_group_owner_id<<endl;//群主 的id
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "insert into T_GROUP_NOTIFICATION(f_group_owner_id,f_user_id,f_type,f_group_id)values('%d','%d','%s','%d');", std::stoi(temp_group_owner_id), user_id,"add",group_id);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql insert notification error" << endl;
        return ;
    }
    else
    {
        cout << "mysql insert notification success" << endl;
    }
}

//通过群Id查找群
void ChatDataBase::my_database_search_group(Group &gp, int groupid) {
    char sql[1024] = {0};
    sprintf(sql, "select * from T_GROUP where T_GROUP.f_group_id='%d'", groupid);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        for(int i=0;i<numField;i++)
        {
            if(i==0)
                gp.groupId = stoi(row[0]);
            if(i==1)
                gp.name=row[1];
            if(i==2)
                gp.ownerId = stoi(row[2]);
        }
    }
}

//用户加入群聊
void ChatDataBase::my_database_user_add_group(int groupid, int userid) {
    char sql[128];
    memset(sql,0,sizeof(sql));
    sprintf(sql,"insert into T_GROUP_MEMBER(f_group_id,f_user_id) values('%d','%d')",groupid,userid);
    if(mysql_query(mysql,sql) !=0)
    {
        cout<<"insert failed"<<endl;
    }
}

void ChatDataBase::my_database_add_new_group(string group_name, int ownerid,int &group_id)
{
    char sql[128] = {0};
    sprintf(sql, "insert into T_GROUP(f_group_name,f_group_owner_id) values('%s','%d');", group_name.c_str(),ownerid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query insert error" << endl;
    }
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select max(f_group_id) from T_GROUP;");
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    cout<<row[0]<<endl;
    string temp = row[0];
    group_id=stoi(temp);
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "insert into T_GROUP_MEMBER(f_group_id,f_user_id) values('%d','%d');", std::stoi(temp),ownerid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query insert error" << endl;
    }
}

void ChatDataBase::my_database_get_group_user(int groupid, vector<User> &ur) {
    char sql[1024] = {0};
    sprintf(sql, "select DISTINCT(f_user_id) from T_GROUP_MEMBER where f_group_id ='%d';", groupid);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    vector<int> temp;
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        temp.push_back(stoi(row[0]));
    }
    User ur_temp;
    vector<int>::iterator iter=temp.begin();
    while(iter!=temp.end()){
        my_database_user_information(ur_temp,*iter);
        ur.push_back(ur_temp);
        iter++;
    }
}

void ChatDataBase::my_database_get_group_chat_msg(int groupid,int userid,vector<Message> &ms) {
    char sql[1024] = {0};
    sprintf(sql, "select * from T_GROUP_MESSAGE where f_groupid='%d'", groupid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return;
    }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    Message temp;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        for(int i=0;i<numField;i++) {
            if(i==1){temp.sendId=stoi(row[i]);}
            if(i==2){temp.receiveId=stoi(row[i]);}
            if(i==3){temp.content=row[i];}
            if(i==4){temp.time=row[i];}
            if(i==5){temp.type=stoi(row[i]);}
        }
        ms.push_back(temp);
    }
    memset(sql,0,sizeof(sql));
    sprintf(sql,"update T_GROUP_MEMBER SET f_msg_num=0 WHERE f_group_id='%d' and f_user_id='%d';",groupid,userid);
    if (mysql_query(mysql, sql) != 0)
    {
        cout << "mysql_query error" << endl;
        return;
    }
}

//在群消息记录中一条消息，如果用户没上线，增加其离线消息数
void ChatDataBase::my_database_group_msg_insert(Message ms) {
    char sql[1024] = {0};
    sprintf(sql, "INSERT into T_GROUP_MESSAGE(f_senderid,f_groupid,f_msgcontent,f_sendtime,f_type) VALUES('%d','%d','%s','%s','%d');", ms.sendId,ms.receiveId,ms.content.c_str(),ms.time.c_str(),ms.type);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    vector<int> ur_temp;
    memset(sql,0,sizeof(sql));
    sprintf(sql,"select T_USER.f_user_id from T_USER,T_GROUP_MEMBER where T_USER.f_user_id=T_GROUP_MEMBER.f_user_id \n"
                "and T_GROUP_MEMBER.f_group_id='%d' and T_USER.f_online=0 and T_USER.f_user_id!='%d';",ms.receiveId,ms.sendId);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    auto res = mysql_store_result(mysql);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        ur_temp.push_back(stoi(row[0]));
    }
    vector<int>::iterator iter = ur_temp.begin();
    while(iter!=ur_temp.end())
    {
        memset(sql,0,sizeof(sql));
        sprintf(sql,"UPDATE T_GROUP_MEMBER SET f_msg_num=f_msg_num+1 where f_group_id='%d' and f_user_id='%d';",ms.receiveId,*iter);
        if (mysql_query(mysql,sql))
        {
            cout<<"Query Error: "<<mysql_error(mysql);
            return ;
        }
        iter++;
    }
}

//通过一个用户id获取该用户加入的所有群聊，并获取相应的历史消息数
void ChatDataBase::my_database_get_group(vector<Group> &vgp, vector<int> &group_message, int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select T_GROUP.f_group_id,T_GROUP.f_group_name,T_GROUP.f_group_owner_id,T_GROUP_MEMBER.f_msg_num from T_GROUP_MEMBER,T_GROUP where T_GROUP.f_group_id = T_GROUP_MEMBER.f_group_id and f_user_id='%d';",userid);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
        return ;
    }
    auto res = mysql_store_result(mysql);
    auto numField = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(res))!=NULL)
    {
        Group temp;
        for(int i=0;i<numField;i++)
        {
            if(i==0)
                temp.groupId=stoi(row[0]);
            if(i==1)
                temp.name=row[1];
            if(i==2)
                temp.ownerId=stoi(row[2]);
        }
        vgp.push_back(temp);
        group_message.push_back(stoi(row[3]));
    }
}

bool ChatDataBase::my_database_in_group(int userId, int groupId) {
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM T_GROUP_MEMBER WHERE T_GROUP_MEMBER.f_group_id = '%d' AND T_GROUP_MEMBER.f_user_id = '%d';", groupId, userId);
    if (mysql_query(mysql,sql))
    {
        cout<<"Query Error: "<<mysql_error(mysql);
    }
    auto res = mysql_store_result(mysql);
    int  num_rows = mysql_num_rows(res); 
    if (num_rows) {  //已经在群里
        return true;
    } else { //不再群里
        return false;
    }
}