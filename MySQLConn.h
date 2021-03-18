#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <iostream>
#include <memory>
#include <mutex>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <mysql-cppconn-8/mysql/jdbc.h>
#include <nlohmann/json_fwd.hpp>

#include "Config.h"
#include "int.h"

using DriverPtr = sql::mysql::MySQL_Driver*;
using ConnectionPtr = std::unique_ptr<sql::Connection>;
using StatementPtr = std::unique_ptr<sql::Statement>;
using PreparedStatementPtr = std::unique_ptr<sql::PreparedStatement>;
using ResultSetPtr = std::unique_ptr<sql::ResultSet>;
using ResultSetMetaDataPtr = sql::ResultSetMetaData*;

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec);

std::ostream& operator<<(std::ostream& os, const ResultSetPtr& resPtr);

class MySQLConn {
public:
    MySQLConn(const nlohmann::json& json = Config::get().json());

    bool execute(std::string_view sql);
    ResultSetPtr executeQuery(std::string_view sql);
    template <typename... Tys>
    ResultSetPtr executeQuery(std::string_view sql, Tys&&... args);

private:
    template <typename Ty>
    static void fill(PreparedStatementPtr& pStmt, size_t index, Ty&& arg);
    // template <typename Ty, typename... Tys>
    // static void helper(PreparedStatementPtr& pStmt, size_t index, Ty&& arg, Tys&&... args);
    // template <typename Ty>
    // static void helper(PreparedStatementPtr& pStmt, size_t index, Ty&& arg);

    ConnectionPtr connection;
    std::mutex mutex;
};

template <typename Ty>
void MySQLConn::fill(PreparedStatementPtr& pStmt, size_t index, Ty&& arg) {
    if constexpr (std::is_convertible_v<Ty, const sql::SQLString&>) {
        pStmt->setString(index, std::forward<Ty>(arg));
    } else if constexpr (std::is_same_v<Ty, int32>) {
        pStmt->setInt(index, std::forward<Ty>(arg));
    } else if constexpr (std::is_same_v<Ty, uint32>) {
        pStmt->setUInt(index, std::forward<Ty>(arg));
    } else if constexpr (std::is_same_v<Ty, int64>) {
        pStmt->setInt64(index, std::forward<Ty>(arg));
    } else if constexpr (std::is_same_v<Ty, uint64>) {
        pStmt->setUInt64(index, std::forward<Ty>(arg));
    } else if constexpr (std::is_same_v<Ty, double>) {
        pStmt->setDouble(index, std::forward<Ty>(arg));
    }
}

template <typename... Tys>
ResultSetPtr MySQLConn::executeQuery(std::string_view sql, Tys&&... args) {
    std::lock_guard<std::mutex> guard{mutex};

    PreparedStatementPtr pStmt{connection->prepareStatement(sql.data())};

    size_t index = 0;
    (..., fill(pStmt, ++index, std::forward<Tys>(args)));

    ResultSetPtr res{pStmt->executeQuery()};

    return res;
}

// template <typename Ty, typename... Tys>
// void MySQLConn::helper(PreparedStatementPtr& pStmt, size_t index, Ty&& arg, Tys&&... args) {
//     fill(pStmt, index, std::forward<Ty>(arg));

//     helper(pStmt, index + 1, std::forward<Tys>(args)...);
// }

// template <typename Ty>
// void MySQLConn::helper(PreparedStatementPtr& pStmt, size_t index, Ty&& arg) {
//     fill(pStmt, index, std::forward<Ty>(arg));
// }

#endif
