#include "../utility/common.h"
#include "json/json.h"

/*
struct User{
    int userId;
    string userName;
    string iconStr;
    string desc;
    int online;
};
*/

Json::Value userToJsonObj(User user) {
    Json::Value obj;
    obj["userId"] = user.userId;
    obj["userName"] = user.userName;
    obj["iconStr"] = user.iconStr;
    obj["desc"] = user.desc;
    obj["online"] = user.online;
    return obj;
}

/*
struct Message {
    int sendId;
    int receiveId;
    int type;
    string content;
    string date;
};
*/
Json::Value messageToJsonObj(Message msg) {
    Json::Value obj;
    obj["sendId"] = msg.sendId;
    obj["receiveId"] = msg.receiveId;
    obj["type"] = msg.type;
    obj["content"] = msg.content;
    obj["time"] = msg.time;
    return obj;
}

/*
struct Group {
    int ownerId;
    int groupId;
    string name;
};
*/

Json::Value groupToJsonObj(const Group &g) {
    Json::Value obj;
    obj["ownerId"] = g.ownerId;
    obj["groupId"] = g.groupId;
    obj["groupName"] = g.name;
    return obj;
}