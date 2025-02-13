include("../cmake/LoadGoogleTests.cmake")

add_executable(testing Tests.cpp)

target_link_libraries(testing PRIVATE processppl GTest::gtest_main)
target_include_directories(testing PUBLIC "../include"})

include(GoogleTest)
gtest_discover_tests(testing)