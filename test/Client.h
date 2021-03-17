#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <string_view>

#include <muduo/net/TcpClient.h>

#include "BikeSharing.pb.h"
#include "Messages.h"
#include "ProtobufCodec.h"
#include "ProtobufDispatcher.h"

// Client is for testing
class Client {
public:
    Client(muduo::net::EventLoop* loop,
           const muduo::net::InetAddress& serverAddr,
           std::string_view nameArg = "test");

    void connect();

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onUnknownMessageType(muduo::net::TcpConnectionPtr conn,
                              MessagePtr message,
                              muduo::Timestamp);
    void onMobileRsp(muduo::net::TcpConnectionPtr conn, MobileRspPtr rsp, muduo::Timestamp);
    void onLoginRsp(muduo::net::TcpConnectionPtr conn, LoginRspPtr rsp, muduo::Timestamp);

    muduo::net::TcpClient client;

    ProtobufDispatcher dispatcher;
    ProtobufCodec codec;
    std::string mobile;
};

#endif
