#include <functional>

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>

#include "Server.h"
#include "Service.h"

using namespace std::placeholders;

Server::Server(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr,
               std::string nameArg)
    : s_(loop, listenAddr, std::move(nameArg)),
      dispatcher_(std::bind(&Server::onUnknownMessageType_, this, _1, _2, _3)),
      codec_(std::bind(&ProtobufDispatcher::onProtobufMessage, dispatcher_, _1, _2, _3)) {
    s_.setConnectionCallback(std::bind(&Server::onConnection, this, _1));
    s_.setMessageCallback(std::bind(&ProtobufCodec::onMessage, codec_, _1, _2, _3));
}

void Server::addService(Service& service) {
    service.registerInDispatcher(dispatcher_);
}

void Server::start() {
    s_.start();
}

void Server::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG_INFO << "Connected by " << conn->peerAddress().toIpPort();
    } else {
        LOG_INFO << "Disconnected by " << conn->peerAddress().toIpPort();
        conn->shutdown();
    }
}

void Server::onUnknownMessageType_(muduo::net::TcpConnectionPtr conn,
                                   MessagePtr message,
                                   muduo::Timestamp) {
    LOG_WARN << "Unknown message yype";

    if (conn->connected()) {
        conn->shutdown();
    }
}
