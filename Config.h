#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>
#include <string_view>

class Config {
public:
    static Config& get();

    void init(std::string_view path);
    nlohmann::json& json();

private:
    Config() = default;

    nlohmann::json j;
};

#endif
