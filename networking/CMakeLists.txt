cmake_minimum_required(VERSION 3.27)
project(networking)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.85.0 REQUIRED)

find_package(libpqxx CONFIG REQUIRED)

file(GLOB SOURCES
        src/server/*.cpp
        src/client/*.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${Boost_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
target_link_libraries(${PROJECT_NAME} PRIVATE wsock32)

target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx::pqxx)

add_definitions(-D_WIN32_WINNT=0x0601)