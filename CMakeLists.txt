cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(tinyRedis VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找所有数据结构的源文件
file(GLOB DATA_STRUCTURE_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Server 可执行文件
add_executable(tinyRedis_server ${PROJECT_SOURCE_DIR}/server/server.cpp ${DATA_STRUCTURE_SOURCES})

# Client 可执行文件
add_executable(tinyRedis_client ${PROJECT_SOURCE_DIR}/client/client.cpp ${DATA_STRUCTURE_SOURCES})
