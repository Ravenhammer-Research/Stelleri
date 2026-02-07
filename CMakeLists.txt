cmake_minimum_required(VERSION 3.25)
project(netcli VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# No external line-editing dependency: use simple std::getline-based CLI.

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(net ${SOURCES})
target_include_directories(net PRIVATE include)

## No libedit/readline linking required.

target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)

install(TARGETS net DESTINATION bin)
