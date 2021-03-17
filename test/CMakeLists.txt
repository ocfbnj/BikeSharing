add_executable(testServer testServer.cpp Client.cpp)
target_link_libraries(testServer
    proto)
target_include_directories(testServer PRIVATE ${CMAKE_SOURCE_DIR})
