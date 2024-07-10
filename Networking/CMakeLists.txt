cmake_minimum_required(VERSION 3.27)
project(Networking)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.85.0 REQUIRED)

file(GLOB SOURCES src/*)

add_library(${PROJECT_NAME} ${SOURCES}
        src/server/Server.cpp
        include/server/Server.h
        src/server/Connection.cpp
        include/server/Connection.h
        src/client/Client.cpp
        include/client/Client.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${Boost_LIBRARIES}
)
target_link_libraries(${PROJECT_NAME} ws2_32)
target_link_libraries(${PROJECT_NAME} wsock32)

add_definitions(-D_WIN32_WINNT=0x0601)