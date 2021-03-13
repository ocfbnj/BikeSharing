#include <muduo/net/EventLoop.h>

#include "LoginService.h"
#include "Server.h"

int main(int argc, char* argv[]) {
    muduo::net::EventLoop loop;

    muduo::net::InetAddress addr{8080};
    Server server{&loop, addr, "BikeSharing"};

    LoginService loginService;
    server.addService(loginService);

    server.start();

    loop.loop();
}
