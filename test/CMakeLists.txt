cmake_minimum_required(VERSION 3.27)
project(test)

find_package(Boost 1.85.0 REQUIRED)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
        command_handler_test.cpp
)

add_executable(run_tests ${TEST_SOURCES})

target_include_directories(run_tests PUBLIC
        networking
)

target_link_libraries(run_tests PUBLIC
        networking
        gtest
        gtest_main
)
