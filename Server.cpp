#include <functional>

#include <muduo/base/Logging.h>

#include "Server.h"
#include "Service.h"

using namespace std::placeholders;

Server::Server(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr,
               std::string nameArg)
    : s_(loop, listenAddr, std::move(nameArg)),
      dispatcher_(std::bind(&Server::onUnknownMessageType_, this, _1, _2, _3)) {
}

void Server::addService(Service& service) {
    service.registerInDispatcher(dispatcher_);
}

void Server::start() {
    s_.start();
}

void Server::onUnknownMessageType_(muduo::net::TcpConnectionPtr conn,
                                   MessagePtr message,
                                   muduo::Timestamp) {
    LOG_WARN << "Unknown message yype";

    if (conn->connected()) {
        conn->shutdown();
    }
}
