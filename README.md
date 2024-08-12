# Network task tracker application on C++

## Introduction
This project is a TCP/IP application written in C++ using the Boost.Asio library. App is designed to provide effective networking Task tracking between remote users.

## Features
- **Asynchronous I/O operations** using Boost.Asio.
- **High performance** with low latency.
- **Console UI** to interact with app
  - All clients can authorize as user with username
  - User can create or edit task with following attributes: title, description, deadline date and participants (users)
  - `list` command display a list of all tasks
  - Users can complete tasks together with participants and track the actions of others (editing, completing, deleting and etc)
- **CRUD operations** using postgreSQL database
- **snake_case** code style

## Requirements
- C++20 (or higher)
- Boost library (version 1.85.0 or higher)
- `libpqxx` api for PostgreSQL
- CMake (version 3.10 or higher)
- A compatible C++ compiler (GCC, Clang, MSVC, etc.)

## Installation
### Step 1
Clone repository
```sh
git clone https://github.com/dwuiem/network-task-tracking.git
cd network-task-tracking
```

### Step 2
Download a boost library from https://www.boost.org/users/download/ or use `vcpkg` package manager
Also add `libpqxx` using `vcpkg`

### Step 3
Use CMake to build project
``` sh
mkdir build
cd build
cmake ..
make
```
## Usage
After building project you will have two executable files in `/server` and in `/client`

## Project Structure
``` graphql
.idea/                  # IDE configuration files
client/                 # Client main
  main.cpp
  CMakeLists.txt
networking/             # Networking utilities and components
  include/ 
    server/             # Headers for server
      ...
    client/             # Headers for client
      ...
  src/
    server/             # Source for server
      ...
    client/             # Source for client
      ...
  CMakeLists.txt
server/                 # Server main
  main.cpp
  CMakeLists.txt           
cmake-build-debug/      # Build output directory
CMakeLists.txt          # CMake build configuration file
```
