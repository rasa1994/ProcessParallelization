add_executable(testing Tests.cpp)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(testing PRIVATE processppl GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

target_include_directories(testing PUBLIC "../include"})

include(GoogleTest)
gtest_discover_tests(testing)