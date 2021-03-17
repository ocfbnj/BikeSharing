#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <memory>

#include <mysql-cppconn-8/mysql/jdbc.h>
#include <nlohmann/json_fwd.hpp>

using MySQLDriverPtr = sql::mysql::MySQL_Driver*;
using MySQLConnectionPtr = std::unique_ptr<sql::Connection>;

class MySQLConn {
public:
    static MySQLConn& get();
    void init(const nlohmann::json& json);

private:
    MySQLConn() = default;

    MySQLConnectionPtr connection;
};

#endif
