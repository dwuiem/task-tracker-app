# Network Task-manager application on C++

## Pay attention
Now this project is still under active development. Please check back regularly for updates.

## Introduction
This project is a TCP/IP application written in C++ using the Boost.Asio library. App is designed to provide effective networking Task tracking between remote users.

## Features
- **Asynchronous I/O operations** using Boost.Asio.
- **High performance** with low latency.
- **Console UI** to interact with app
- **CRUD operations** using database

## Requirements
- C++20 (or higher)
- Boost library (version 1.85.0 or higher)
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

### Step 3
Use CMake to build project
``` sh
mkdir build
cd build
cmake ..
make
```
## Usage
After building project you will have two executable files in `/Server` and in `/Client`

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
