LDFLAGS+=-pthread
CC=g++
OBJS=server.o conn.o ChatDataBase.o common.o jsoncpp.cpp DbPool.o
CPPFLAGS+=-Wall -g -std=c++11 -I ../utility -I ../database
MYSQLFLAG=`mysql_config --cflags --libs`

server: $(OBJS)
	$(CC) $(OBJS) $(CPPFLAGS) $(LDFLAGS) $(MYSQLFLAG) -o server

server.o: server.cpp
	$(CC) server.cpp $(CPPFLAGS) -c -lpthread -o server.o

conn.o: conn.cpp
	$(CC) conn.cpp $(CPPFLAGS) -c -o conn.o

ChatDataBase.o: ChatDataBase.cpp
	$(CC) ChatDataBase.cpp $(CPPFLAGS) -c $(MYSQLFLAG) -o ChatDataBase.o  

common.o: common.cpp
	$(CC) common.cpp $(CPPFLAGS) -c -o common.o

DbPool.o: DbPool.cpp
	$(CC) DbPool.cpp $(CPPFLAGS) -c $(MYSQLFLAG) -o DbPool.o  

clean:
	rm *.o server -rf
