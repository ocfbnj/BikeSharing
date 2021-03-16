#include <functional>
#include <thread>

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>

#include "Server.h"
#include "Service.h"

using namespace std::placeholders;

Server::Server(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr,
               const std::string& nameArg)
    : server(loop, listenAddr, nameArg),
      dispatcher(std::bind(&Server::onUnknownMessageType, this, _1, _2, _3)),
      codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher, _1, _2, _3)) {

    server.setThreadNum(2 * std::thread::hardware_concurrency());
    server.setConnectionCallback(std::bind(&Server::onConnection, this, _1));
    server.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &codec, _1, _2, _3));
}

void Server::addService(Service& service) {
    service.registerInDispatcher(dispatcher);
}

void Server::start() {
    server.start();
}

void Server::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

void Server::onUnknownMessageType(const muduo::net::TcpConnectionPtr& conn,
                                  const MessagePtr& message,
                                  muduo::Timestamp) {
    LOG_WARN << "Unknown message type: " << message->GetTypeName();

    if (conn->connected()) {
        conn->shutdown();
    }
}
