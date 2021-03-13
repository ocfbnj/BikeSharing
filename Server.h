#ifndef SERVER_H
#define SERVER_H

#include <string>

#include <muduo/net/TcpServer.h>

#include "ProtobufDispatcher.h"

class Service;

class Server {
public:
    Server(muduo::net::EventLoop* loop,
           const muduo::net::InetAddress& listenAddr,
           std::string nameArg);

    void addService(Service& service);
    void start();

private:
    void onUnknownMessageType_(muduo::net::TcpConnectionPtr conn,
                               MessagePtr message,
                               muduo::Timestamp);

    muduo::net::TcpServer s_;

    ProtobufDispatcher dispatcher_;
};

#endif
