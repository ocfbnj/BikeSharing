#include <functional>

#include <muduo/base/Logging.h>

#include "Client.h"

using namespace std::placeholders;

Client::Client(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& serverAddr,
               std::string_view nameArg)
    : client(loop, serverAddr, nameArg.data()),
      dispatcher(std::bind(&Client::onUnknownMessageType, this, _1, _2, _3)),
      codec(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher, _1, _2, _3)) {

    client.setConnectionCallback(std::bind(&Client::onConnection, this, _1));
    client.setMessageCallback(std::bind(&ProtobufCodec::onMessage, &codec, _1, _2, _3));

    dispatcher.registerMessageCallback<BikeSharing::MobileRsp>(
        std::bind(&Client::onMobileRsp, this, _1, _2, _3));
    dispatcher.registerMessageCallback<BikeSharing::LoginRsp>(
        std::bind(&Client::onLoginRsp, this, _1, _2, _3));

    mobile = "12345678901";
}

void Client::connect() {
    client.connect();
}

void Client::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        BikeSharing::MobileReq req;
        req.set_mobile(mobile);

        ProtobufCodec::send(conn, req);
    } else {
        conn->shutdown();
    }
}

void Client::onUnknownMessageType(muduo::net::TcpConnectionPtr conn,
                                  MessagePtr message,
                                  muduo::Timestamp) {
    LOG_WARN << "Unknown message type" << message->GetTypeName();

    if (conn->connected()) {
        conn->shutdown();
    }
}

void Client::onMobileRsp(muduo::net::TcpConnectionPtr conn, MobileRspPtr rsp, muduo::Timestamp) {
    LOG_DEBUG << "onMobileRsp():\n"
              << rsp->DebugString();

    BikeSharing::LoginReq req;
    req.set_mobile(mobile);
    req.set_vcode(rsp->vcode());

    ProtobufCodec::send(conn, req);
}

void Client::onLoginRsp(muduo::net::TcpConnectionPtr conn, LoginRspPtr rsp, muduo::Timestamp) {
    LOG_DEBUG << "onLoginRsp():\n"
              << rsp->DebugString();
}
