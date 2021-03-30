//注册OK

{

"cmd": "register",

"userName": "飞翔的企鹅", (string)

"password": 123  (string)

}

{

"cmd": "register-reply",

"userid": 100000, (int)

}

```c++
void my_database_user_password(string, string,int&userid);
```


//登录OK

{

"cmd": "login",

"userid": 100000, (int)

"password": 123 (string)

}

{

"cmd": "login-reply",

"success": 1,

"user": {

"userId": 100,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": true

}

}

```plain
bool my_database_password_correct(int, string); 
// 登录时 online =1；
```
~~//搜索用户~~~~OK~~

~~{~~

~~"cmd":"searchUser"-~~

~~"userId":100000~~

~~}~~

~~{~~

~~"cmd":"searchUser_reply"~~

~~"userId": 100,~~

~~"userName": "名字",~~

~~"iconStr": "/img/1.jpg",~~

~~"desc": "个性签名",~~

~~???"online": true~~

~~}~~

```plain
bool search_user(int,User& ur);
//判断用户存不存在  存在即返回用户所有信息
```

~~//搜索群聊~~

~~{~~

~~"cmd":"searchGroup"~~

~~"groupId":100000~~

~~}~~

~~{~~

~~"cmd":"searchGroup_reply"~~

~~???~~

~~}~~



{

"cmd": "getFriendList-reply",

"user": [

{

"userId": 100,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": true,

"msgNum": 0

},

{

"userId": 101,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": true,

"msgNum": 0

},

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false,

"msgNum": 0

}

]

}

```plain
void my_database_friend_list(std::vector<User> &friendlist,int);
 //返回所有好友的信息
```


//sendMessage

{

"cmd": "sendMessage",

"sendId": 100,

"reciveId": 101,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

}

//用户接受数据

{

"cmd": "receiveMessage",

"sendId": 10000,

"receiveId": 10001,

"type": 1,

"content": "1",

"time": "2021-3-18 12:34"

}



~~//获取离线消息~~

~~{~~

~~"cmd": "getOfflineMessage",~~

~~"userId": 10000~~

~~}~~

~~{~~

~~"cmd": "getOfflineMessage-reply",~~

~~"message": [~~

~~{~~

~~"sendId": 10001,~~

~~"messageNum": 2~~

~~},~~

~~{~~

~~"sendId": 10002,~~

~~"messageNum": 2~~

~~}~~

~~]~~

~~}~~


//离线时客户端向服务器发送该消息，服务器修改用户状态为离线状态

{

"cmd": "offline",

"userId": 10000

}


//-------------------------------添加好友数据格式------------

{

"cmd": "searchUserById",

"userId": 10000

}

{

"cmd": "searchUserById-reply",

"user":

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false

}

}

//10000向10001发送添加好友的请求，如果接受者不在线，将请求存入数据库，如果接受者在线，发送addFriendNotification通知

{

"cmd": "addFriend",

"sendId": 10000,

"receiveId": 10001

}

//1表示请求添加的好友存在，已经发送请求，0表示用户不存在

{

"cmd": "addFriend-reply",

"success": 1

}

{

"cmd": "addFriendNotification",

"user":

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false

}

}


//接受者点击确定，客户端发送如下json

{

"cmd": "addFriendConfirm",

"sendId": 10000,

"receiveId": 10001

}

//根据发送者和接受者是否在线进行处理，不在线，在数据库好友关系表中加入好友即可；在线服务器向客户端发送如下请求

{

"cmd": "addUserToFriendList",

"user":

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false

}

}





//查找聊天记录

{

"cmd": "getMessageRecord",

"userId1": 10000,

"userId2": 10001

}

{

"cmd": "getMessageRecord-reply",

"message": [

{

"sendId": 10000,

"receiveId": 10001,

"type": 1,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

},

{

"sendId": 10001,

"receiveId": 10000,

"type": 1,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

}

]

}


//用户上线，读取所有添加好友的通知

{

"cmd": "readAddFriendNotification",

"userId": 10000

}


{

"cmd": "readAddFriendNotification-reply",

"user": [

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false

},

{

"userId": 102,

"userName": "名字",

"iconStr": "/img/1.jpg",

"desc": "个性签名",

"online": false

}

]

}


//------------------------------------------------群------------------------------------------

//



{

"cmd": "createGroup",

"userId": 10000,

"gourpName": "groupName"

}

//群创建成功发给群主

{

"cmd": "createGroup-reply",

"group":{

"ownerId": 10000,

"groupId": 20000,

"groupName": "groupName"

}

}

{

"cmd": "searchGroupById",

"groupId": 10000,

}

//返回群信息

{

"cmd": "searchGroupById-reply",

"group":{

"ownerId": 10000,

"groupId": 20000,

"groupName": "groupName"

}

}

//申请加入群

{

"cmd": "requestAddGroup",

"userId": 10000,

"groupId": 20000,

"ownerId": 10000

}

//服务器给申请者发送下面的json

{

"cmd": "addGroupToList",

"group":

{

"gourpId": 20000,

"groupName": "名字",

"ownerId": 10000

}

}

//---------------------------获取群列表和获取群成员--------------------------------

{

"cmd": "getGroupList",

"userId": 10000

}

{

"cmd": "getGroupList-reply",

"group": [

{

"gourpId": 20000,

"groupName": "名字",

"ownerId": 10000,

"msgNum": 10

},

{

"gourpId": 20000,

"groupName": "名字",

"ownerId": 10000,

"msgNum": 10

}

]

}

{

"cmd": "openGroup",

"userId": 10000,

"groupId": 20000

}

{

"cmd": "openGroup-reply",

"groupId": 20000,

"userId": 10000,

"user": [

{

"userId": 102,

"userName": "名字",

"iconStr": "/1.png"

},

{

"userId": 102,

"userName": "名字",

"iconStr": "/1.png"

}

],

"msg": [

{

"sendId": 10000,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

},

{

"sendId": 10001,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

}

]

}


{

"cmd": "sendGroupMessage",

"sendId": 10000,

"groupId": 20000,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

}

//给群中在线成员发送如下json，给不在线的群成员增加未读消息

{

"cmd": "receiveGroupMessage",

"sendId": 10000,

"groupId": 20000,

"content": "消息内容",

"time": "2021年3月16日 15:03:40"

}


