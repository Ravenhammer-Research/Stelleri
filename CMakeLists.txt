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

project(stelleri VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Client type: "lite" (default, FreeBSD system calls) or "netconf" (stub)
# Renamed to STELLERI per request.
set(STELLERI "lite" CACHE STRING "Configuration backend: lite or netconf")
set_property(CACHE STELLERI PROPERTY STRINGS lite netconf)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# When building the netconf client, exclude all System*.cpp files
if(STELLERI STREQUAL "netconf")
  file(GLOB_RECURSE SYSTEM_SOURCES "src/System*.cpp")
  list(REMOVE_ITEM SOURCES ${SYSTEM_SOURCES})
endif()

# Exclude internal netconf implementation and server sources unless netconf
# client is enabled. This prevents accidental recursive inclusion of
# `src/system/netconf` and `src/server` when building the default "lite"
# configuration.
file(GLOB_RECURSE NETCONF_CFGMGR "src/system/netconf/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "src/server/*.cpp")
if(NOT STELLERI STREQUAL "netconf")
  list(REMOVE_ITEM SOURCES ${NETCONF_CFGMGR} ${SERVER_SOURCES})
endif()

add_executable(net ${SOURCES})
target_include_directories(net PRIVATE include)

if(STELLERI STREQUAL "netconf")
  target_compile_definitions(net PRIVATE STELLERI_NETCONF=1)
endif()

target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)

# Link libedit for interactive command-line editing
find_library(EDIT_LIBRARY NAMES edit REQUIRED)
target_link_libraries(net PRIVATE ${EDIT_LIBRARY})

# Link libipsec for ipsec_set_policy / ipsec_get_policylen / ipsec_dump_policy
if(STELLERI STREQUAL "lite")
  find_library(IPSEC_LIBRARY NAMES ipsec REQUIRED)
  target_link_libraries(net PRIVATE ${IPSEC_LIBRARY})
endif()

install(TARGETS net DESTINATION bin)

# Prefer BSD make (`bmake`) if available. Do not override an explicit
# `CMAKE_MAKE_PROGRAM` set by the user or generator, but set it when absent.
if(NOT DEFINED CMAKE_MAKE_PROGRAM OR CMAKE_MAKE_PROGRAM STREQUAL "")
  find_program(BMAKE_EXE bmake)
  if(BMAKE_EXE)
    set(CMAKE_MAKE_PROGRAM "${BMAKE_EXE}" CACHE FILEPATH "Make program" FORCE)
  endif()
endif()
