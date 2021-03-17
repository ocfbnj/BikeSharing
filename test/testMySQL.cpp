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

    auto res = MySQLConn::get().executeQuery("SELECT * FROM user_info");
    std::cout << res << '\n';

    res = MySQLConn::get().executeQuery("SELECT * FROM user_info WHERE id BETWEEN ? AND ?", 1, 10);
    std::cout << res << '\n';
}
