#ifndef LOGIN_SERVICE_H
#define LOGIN_SERVICE_H

#include <muduo/net/TcpConnection.h>

#include "Messages.h"
#include "Service.h"

class LoginService : public Service {
public:
    void registerInDispatcher(ProtobufDispatcher& dispatcher);

    void onMobileReq(muduo::net::TcpConnectionPtr conn, MobileReqPtr message, muduo::Timestamp);
    void onLoginReq(muduo::net::TcpConnectionPtr conn, LoginReqPtr message, muduo::Timestamp);

private:
};

#endif
