#include <fstream>

#include "Config.h"

Config& Config::get() {
    static Config config;

    return config;
}

void Config::init(std::string_view path) {
    std::ifstream ifs{path.data()};
    ifs >> j;
}

nlohmann::json& Config::json() {
    return j;
}
