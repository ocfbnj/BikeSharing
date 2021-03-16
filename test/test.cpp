#include "Client.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

int main() {
    muduo::g_logLevel = muduo::Logger::DEBUG;

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr{"127.0.0.1", 8080};

    // for (int i = 0; i != 1000'000; i++) {
    Client client{&loop, addr};
    client.connect();
    // }

    loop.loop();
}