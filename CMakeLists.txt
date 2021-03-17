cmake_minimum_required(VERSION 3.16)

project(BikeSharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# ./vcpkg install protobuf
find_package(protobuf CONFIG REQUIRED)

# ./vcpkg install openssl
find_package(OpenSSL REQUIRED)

# From github
# See https://github.com/chenshuo/muduo
find_library(MUDUO_BASE muduo_base)
find_library(MUDUO_NET muduo_net)

# From MySQL APT Repository
# See https://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/
find_library(MYSQL_STATIC mysqlcppconn-static)

# Protobuf codec and dispatcher
add_subdirectory(protobuf)

# Testing
add_subdirectory(test)

set(headers
    int.h
    Server.h
    Service.h
    LoginService.h)

set(sources
    main.cpp
    Server.cpp
    LoginService.cpp)

set(deps
    ${MUDUO_BASE} 
    ${MUDUO_NET}
    ${MYSQL_STATIC}
    OpenSSL::SSL
    OpenSSL::Crypto
    proto)

if(UNIX)
    list(APPEND deps resolv)
endif(UNIX)

add_executable(${CMAKE_PROJECT_NAME} ${headers} ${sources})

# We want to use static library
# See https://dev.mysql.com/doc/connector-cpp/8.0/en/connector-cpp-apps-general-considerations.html#connector-cpp-apps-static-library
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE STATIC_CONCPP CPPCONN_PUBLIC_FUNC=)
target_link_libraries(${CMAKE_PROJECT_NAME} ${deps})

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
endif(UNIX)
