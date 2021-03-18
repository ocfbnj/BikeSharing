#include <fmt/format.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include "Config.h"
#include "LoginService.h"
#include "MySQLConn.h"
#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << fmt::format("Usage: {} <path to config file>\n", argv[0]);
        return 0;
    }

    try {
        Config::get().init(argv[1]);
    } catch (const std::exception& e) {
        std::cout << fmt::format("At initialization: {}\n", e.what());
        return 1;
    }

    muduo::g_logLevel = muduo::Logger::DEBUG;

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr{8080};

    Server server{&loop, addr, "BikeSharing"};

    LoginService loginService;
    server.addService(loginService);
    server.start();

    loop.loop();
}
