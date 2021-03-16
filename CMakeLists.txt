cmake_minimum_required(VERSION 3.16)

project(BikeSharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(protobuf CONFIG REQUIRED)
find_library(MUDUO_BASE muduo_base)
find_library(MUDUO_NET muduo_net)

add_subdirectory(protobuf)
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
    proto)

add_executable(${CMAKE_PROJECT_NAME} ${headers} ${sources})
target_link_libraries(${CMAKE_PROJECT_NAME} ${deps})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

if (UNIX)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
endif(UNIX)
