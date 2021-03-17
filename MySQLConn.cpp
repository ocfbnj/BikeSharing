#include <string>

#include <nlohmann/json.hpp>

#include "MySQLConn.h"

MySQLConn& MySQLConn::get() {
    static MySQLConn conn;
    return conn;
}

void MySQLConn::init(const nlohmann::json& json) {
    MySQLDriverPtr driver{sql::mysql::get_driver_instance()};

    std::string hostname = json["mysql.hostname"];
    std::string username = json["mysql.username"];
    std::string password = json["mysql.password"];
    connection.reset(driver->connect(hostname, username, password));

    std::string schema = json["mysql.schema"];
    connection->setSchema(schema);
}