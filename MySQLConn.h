#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <memory>
#include <string_view>

#include <mysql-cppconn-8/mysql/jdbc.h>
#include <nlohmann/json_fwd.hpp>

using DriverPtr = sql::mysql::MySQL_Driver*;
using ConnectionPtr = std::unique_ptr<sql::Connection>;
using StatementPtr = std::unique_ptr<sql::Statement>;
using ResultSetPtr = std::unique_ptr<sql::ResultSet>;

class MySQLConn {
public:
    static MySQLConn& get();
    void init(const nlohmann::json& json);

    bool execute(std::string_view sql);
    ResultSetPtr executeQuery(std::string_view sql);

private:
    MySQLConn() = default;

    ConnectionPtr connection;
};

#endif
