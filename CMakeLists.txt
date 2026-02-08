cmake_minimum_required(VERSION 3.25)
project(netcli VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Client type: "lite" (default, FreeBSD system calls) or "netconf" (stub)
set(NETCLI_CLIENT "lite" CACHE STRING "Configuration backend: lite or netconf")
set_property(CACHE NETCLI_CLIENT PROPERTY STRINGS lite netconf)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# When building the netconf client, exclude all System*.cpp files
if(NETCLI_CLIENT STREQUAL "netconf")
  file(GLOB_RECURSE SYSTEM_SOURCES "src/System*.cpp")
  list(REMOVE_ITEM SOURCES ${SYSTEM_SOURCES})
endif()

add_executable(net ${SOURCES})
target_include_directories(net PRIVATE include)

if(NETCLI_CLIENT STREQUAL "netconf")
  target_compile_definitions(net PRIVATE NETCLI_NETCONF=1)
else()
  target_compile_definitions(net PRIVATE NETCLI_LITE=1)
endif()

target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)

install(TARGETS net DESTINATION bin)
