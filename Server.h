#ifndef SERVER_H
#define SERVER_H

#include <muduo/net/TcpServer.h>

#include "ProtobufCodec.h"
#include "ProtobufDispatcher.h"

class Service;

class Server {
public:
    Server(muduo::net::EventLoop* loop,
           const muduo::net::InetAddress& listenAddr,
           const std::string& nameArg);

    void addService(Service& service);
    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onUnknownMessageType(const muduo::net::TcpConnectionPtr& conn,
                              const MessagePtr& message,
                              muduo::Timestamp);

    muduo::net::TcpServer server;

    ProtobufDispatcher dispatcher;
    ProtobufCodec codec;
};

#endif
