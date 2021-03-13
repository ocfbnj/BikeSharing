#include <functional>

#include <muduo/base/Logging.h>

#include "LoginService.h"
#include "ProtobufDispatcher.h"

void LoginService::registerInDispatcher(ProtobufDispatcher& dispatcher) {
    using namespace std::placeholders;

    dispatcher.registerMessageCallback<BikeSharing::MobileReq>(
        std::bind(&LoginService::onMobileReq, this, _1, _2, _3));
    dispatcher.registerMessageCallback<BikeSharing::LoginReq>(
        std::bind(&LoginService::onLoginReq, this, _1, _2, _3));
}

void LoginService::onMobileReq(muduo::net::TcpConnectionPtr conn,
                               MobileReqPtr message,
                               muduo::Timestamp) {
    LOG_TRACE << "onMobileReq: ";
    message->PrintDebugString();
}

void LoginService::onLoginReq(muduo::net::TcpConnectionPtr conn,
                              LoginReqPtr message,
                              muduo::Timestamp) {
    LOG_TRACE << "onLoginReq";
    message->PrintDebugString();
}
