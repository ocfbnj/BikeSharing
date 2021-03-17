#include <iomanip>

#include <fmt/format.h>

#include "Config.h"
#include "MySQLConn.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << fmt::format("Usage: {} <path to config file>\n", argv[0]);
        return 0;
    }

    try {
        Config::get().init(argv[1]);
        MySQLConn::get().init(Config::get().json());
    } catch (const std::exception& e) {
        std::cout << fmt::format("At initialization: {}\n", e.what());
        return 1;
    }

    auto res = MySQLConn::get().executeQuery("SELECT * from user_info");

    std::cout.setf(std::ios::left, std::ios::adjustfield);
    while (res->next()) {
        std::cout << std::setw(16) << res->getString("username");
        std::cout << std::setw(16) << res->getString("mobile");
        std::cout << std::setw(16) << res->getString("balance");
        std::cout << std::setw(16) << res->getString("register_time");

        std::cout.put('\n');
    }
}
