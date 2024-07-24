cmake_minimum_required(VERSION 3.27)
project(client)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(networking)
add_subdirectory(client)
add_subdirectory(server)
