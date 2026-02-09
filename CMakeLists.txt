cmake_minimum_required(VERSION 3.25)

# Prefer clang/clang++ when available and the user has not explicitly set a
# compiler. This must be done before the `project()` command so CMake will
# use the chosen compilers during configuration.
if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
  find_program(CLANG_EXE clang)
  find_program(CLANGXX_EXE clang++)
  if(CLANG_EXE AND CLANGXX_EXE)
    set(CMAKE_C_COMPILER "${CLANG_EXE}" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "${CLANGXX_EXE}" CACHE FILEPATH "C++ compiler" FORCE)
  endif()
endif()

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

# Link libedit for interactive command-line editing
find_library(EDIT_LIBRARY NAMES edit REQUIRED)
target_link_libraries(net PRIVATE ${EDIT_LIBRARY})

install(TARGETS net DESTINATION bin)

# Prefer BSD make (`bmake`) if available. Do not override an explicit
# `CMAKE_MAKE_PROGRAM` set by the user or generator, but set it when absent.
if(NOT DEFINED CMAKE_MAKE_PROGRAM OR CMAKE_MAKE_PROGRAM STREQUAL "")
  find_program(BMAKE_EXE bmake)
  if(BMAKE_EXE)
    set(CMAKE_MAKE_PROGRAM "${BMAKE_EXE}" CACHE FILEPATH "Make program" FORCE)
  endif()
endif()
