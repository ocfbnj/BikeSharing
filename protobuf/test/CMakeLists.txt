find_package(GTest CONFIG)

if(GTest_FOUND)
    add_executable(test test.cpp)
    target_link_libraries(test
        GTest::gtest
        GTest::gtest_main
        proto)
endif(GTest_FOUND)
