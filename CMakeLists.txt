cmake_minimum_required(VERSION 3.26)
project(CreateInterfaceRebuild)

set(CMAKE_CXX_STANDARD 23)

find_package(spdlog REQUIRED)

add_library(CreateInterfaceRebuild SHARED library.cpp
        src/utility/console.cpp
        src/utility/memory.cpp
        src/utility/pe.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        spdlog::spdlog_header_only
)