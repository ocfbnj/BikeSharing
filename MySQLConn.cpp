#include <string>

#include <nlohmann/json.hpp>

#include "MySQLConn.h"

MySQLConn& MySQLConn::get() {
    static MySQLConn conn;
    return conn;
}

void MySQLConn::init(const nlohmann::json& json) {
    DriverPtr driver{sql::mysql::get_driver_instance()};

    std::string hostname = json["mysql.hostname"];
    std::string username = json["mysql.username"];
    std::string password = json["mysql.password"];
    connection.reset(driver->connect(hostname, username, password));

    std::string schema = json["mysql.schema"];
    connection->setSchema(schema);
}

bool MySQLConn::execute(std::string_view sql) {
    StatementPtr stmt{connection->createStatement()};

    return stmt->execute(sql.data());
}

ResultSetPtr MySQLConn::executeQuery(std::string_view sql) {
    StatementPtr stmt{connection->createStatement()};
    ResultSetPtr res{stmt->executeQuery(sql.data())};

    return res;
}
