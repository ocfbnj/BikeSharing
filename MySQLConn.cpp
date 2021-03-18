#include <iomanip>
#include <string>

#include <nlohmann/json.hpp>

#include "MySQLConn.h"

constexpr auto Width = 20;

static void printBreak(std::ostream& os, uint32 len, bool newLine = true) {
    os << std::setfill('-') << std::setw(len) << '-';
    if (newLine) {
        os << "-\n";
    } else {
        os << '-';
    }
    os << std::setfill(' ');
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec) {
    for (const std::string& str : vec) {
        os << "| " << std::setw(Width) << str;
    }

    return os << '|';
}

std::ostream& operator<<(std::ostream& os, const ResultSetPtr& resPtr) {
    ResultSetMetaDataPtr metaPtr{resPtr->getMetaData()};
    std::vector<std::string> columnNames;
    std::vector<std::string> columns;

    uint32 columnCount = metaPtr->getColumnCount();
    for (uint32 i = 1; i <= columnCount; i++) {
        columnNames.emplace_back(metaPtr->getColumnName(i));
    }

    uint32 breakLen = columnCount * (Width + 2);

    printBreak(os, breakLen);
    os << columnNames << '\n';
    printBreak(os, breakLen);

    while (resPtr->next()) {
        columns.clear();
        for (uint32 i = 1; i <= columnCount; i++) {
            columns.emplace_back(resPtr->getString(i));
        }

        os << columns << '\n';
    }

    printBreak(os, breakLen, false);

    return os;
}

MySQLConn::MySQLConn(const nlohmann::json& json) {
    DriverPtr driver{sql::mysql::get_driver_instance()};

    std::string hostname = json["mysql.hostname"];
    std::string username = json["mysql.username"];
    std::string password = json["mysql.password"];
    connection.reset(driver->connect(hostname, username, password));

    std::string schema = json["mysql.schema"];
    connection->setSchema(schema);
}

bool MySQLConn::execute(std::string_view sql) {
    std::lock_guard<std::mutex> guard{mutex};

    StatementPtr stmt{connection->createStatement()};

    return stmt->execute(sql.data());
}

ResultSetPtr MySQLConn::executeQuery(std::string_view sql) {
    std::lock_guard<std::mutex> guard{mutex};

    StatementPtr stmt{connection->createStatement()};
    ResultSetPtr res{stmt->executeQuery(sql.data())};

    return res;
}
