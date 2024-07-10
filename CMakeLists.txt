cmake_minimum_required(VERSION 3.27)
project(Client)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Networking)
add_subdirectory(Client)
add_subdirectory(Server)
