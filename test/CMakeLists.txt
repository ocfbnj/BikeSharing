add_executable(testServer testServer.cpp Client.cpp)
target_link_libraries(testServer proto)
target_include_directories(testServer PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(testMySQL
    testMySQL.cpp
    ${CMAKE_SOURCE_DIR}/Config.cpp
    ${CMAKE_SOURCE_DIR}/MySQLConn.cpp)
target_link_libraries(testMySQL
    ${MYSQL_STATIC}
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt
    nlohmann_json::nlohmann_json
    resolv)
target_include_directories(testMySQL PRIVATE ${CMAKE_SOURCE_DIR})
