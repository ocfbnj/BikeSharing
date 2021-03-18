#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <muduo/net/TcpConnection.h>
#include <string>
#include <unordered_map>

#include "Messages.h"
#include "MySQLConn.h"
#include "Service.h"

class LoginService : public Service {
public:
    static constexpr auto vCodeSize = 6;

    void registerInDispatcher(ProtobufDispatcher& dispatcher);

    void onMobileReq(const muduo::net::TcpConnectionPtr& conn,
                     const MobileReqPtr& message,
                     muduo::Timestamp);
    void onLoginReq(const muduo::net::TcpConnectionPtr& conn,
                    const LoginReqPtr& message,
                    muduo::Timestamp);

private:
    MySQLConn dbConn;
    std::unordered_map<std::string, std::string> mobileVCode;
};

#endif
