#include <fmt/format.h>
#include <muduo/net/EventLoop.h>

#include "Config.h"
#include "LoginService.h"
#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << fmt::format("Usage: {} <path to config file>\n", argv[0]);
        return 0;
    }

    Config::get().init(argv[1]);

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr{8080};

    Server server{&loop, addr, "BikeSharing"};

    LoginService loginService;
    server.addService(loginService);
    server.start();

    loop.loop();
}
