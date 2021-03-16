#include <functional>
#include <random>

#include <muduo/base/Logging.h>

#include "LoginService.h"
#include "ProtobufCodec.h"
#include "ProtobufDispatcher.h"

void LoginService::registerInDispatcher(ProtobufDispatcher& dispatcher) {
    using namespace std::placeholders;

    dispatcher.registerMessageCallback<BikeSharing::MobileReq>(
        std::bind(&LoginService::onMobileReq, this, _1, _2, _3));
    dispatcher.registerMessageCallback<BikeSharing::LoginReq>(
        std::bind(&LoginService::onLoginReq, this, _1, _2, _3));
}

void LoginService::onMobileReq(const muduo::net::TcpConnectionPtr& conn,
                               const MobileReqPtr& message,
                               muduo::Timestamp) {
    LOG_DEBUG << "onMobileReq:\n"
              << message->DebugString();

    static std::mt19937 e{std::random_device{}()};
    static std::uniform_int_distribution<char> d{'0', '9'};

    char vCode[vCodeSize + 1]{};

    for (int i = 0; i != vCodeSize; i++) {
        vCode[i] = d(e);
    }

    BikeSharing::MobileRsp rsp;
    rsp.set_rspcode(0);
    rsp.set_vcode(vCode);

    ProtobufCodec::send(conn, rsp);
}

void LoginService::onLoginReq(const muduo::net::TcpConnectionPtr& conn,
                              const LoginReqPtr& message,
                              muduo::Timestamp) {
    LOG_DEBUG << "onLoginReq:\n"
              << message->DebugString();
}
