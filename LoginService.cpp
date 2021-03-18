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

    mobileVCode[message->mobile()] = vCode;

    ProtobufCodec::send(conn, rsp);
}

void LoginService::onLoginReq(const muduo::net::TcpConnectionPtr& conn,
                              const LoginReqPtr& message,
                              muduo::Timestamp) {
    LOG_DEBUG << "onLoginReq:\n"
              << message->DebugString();

    const std::string& mobile = message->mobile();
    auto it = mobileVCode.find(mobile);
    if (it == mobileVCode.end()) {
        LOG_DEBUG << "Mobile error: " << mobile;
        conn->shutdown();
        return;
    }

    if (message->vcode() != it->second) {
        LOG_DEBUG << "Mobile vcode error: " << message->vcode();
        conn->shutdown();
        return;
    }

    // try to register this user
    auto res = dbConn.executeQuery("SELECT id FROM user_info WHERE mobile=?", mobile);
    if (!res->next()) {
        LOG_DEBUG << "The user is not registered";
        dbConn.executeQuery("CALL register_user(?, ?)", "null", mobile);
    } else {
        LOG_DEBUG << "This user is already registered";
    }

    BikeSharing::LoginRsp rsp;
    rsp.set_code(200);
    rsp.set_info("ok");

    ProtobufCodec::send(conn, rsp);
}
