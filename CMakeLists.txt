cmake_minimum_required(VERSION 3.25)

# Prefer clang/clang++ when available and the user has not explicitly set a
# compiler. This must be done before the `project()` command so CMake will
# use the chosen compilers during configuration.
if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
  find_program(CLANG_EXE clang)
  find_program(CLANGXX_EXE clang++)
  if(CLANG_EXE AND CLANGXX_EXE)
    set(CMAKE_C_COMPILER "${CLANG_EXE}" CACHE FILEPATH "C compiler")
    set(CMAKE_CXX_COMPILER "${CLANGXX_EXE}" CACHE FILEPATH "C++ compiler")
  endif()
endif()

message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")

project(stelleri VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Client type: "lite" (default, FreeBSD system calls) or "netconf" (stub)
# Renamed to STELLERI per request.
set(STELLERI "lite" CACHE STRING "Configuration backend: lite or netconf")
set_property(CACHE STELLERI PROPERTY STRINGS lite netconf)

message(STATUS "STELLERI mode: ${STELLERI}")

file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "src/server/*.cpp")
file(GLOB_RECURSE NETCONF_CFGMGR "src/system/netconf/*.cpp")

# By default ("lite") exclude netconf/system and server sources. When
# building with STELLERI=netconf both client and server components are
# produced and the netconf implementation is included.
if(NOT STELLERI STREQUAL "netconf")
  list(REMOVE_ITEM ALL_SOURCES ${NETCONF_CFGMGR} ${SERVER_SOURCES})
endif()

# `net` is the primary client executable: build from all sources except
# the server sources (server/*). When netconf is enabled the netconf
# system implementation will be included above.
set(NET_SOURCES ${ALL_SOURCES})
list(REMOVE_ITEM NET_SOURCES ${SERVER_SOURCES})

add_executable(net ${NET_SOURCES})
target_include_directories(net PRIVATE include)

# Build the server daemon only when netconf is enabled
if(STELLERI STREQUAL "netconf")
  add_executable(netd ${SERVER_SOURCES})
  target_include_directories(netd PRIVATE include)

  # Expose the netconf compile definition to both targets
  target_compile_definitions(net PRIVATE STELLERI_NETCONF=1)
  target_compile_definitions(netd PRIVATE STELLERI_NETCONF=1)

  # Use pkg-config to locate libyang and libnetconf2 and link them into
  # both targets.
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBYANG REQUIRED libyang)
  pkg_check_modules(LIBNETCONF2 REQUIRED libnetconf2)

  message(STATUS "LIBYANG include dirs: ${LIBYANG_INCLUDE_DIRS}")
  message(STATUS "LIBYANG library dirs: ${LIBYANG_LIBRARY_DIRS}")
  message(STATUS "LIBYANG libraries: ${LIBYANG_LIBRARIES}")

  message(STATUS "LIBNETCONF2 include dirs: ${LIBNETCONF2_INCLUDE_DIRS}")
  message(STATUS "LIBNETCONF2 library dirs: ${LIBNETCONF2_LIBRARY_DIRS}")
  message(STATUS "LIBNETCONF2 libraries: ${LIBNETCONF2_LIBRARIES}")

  target_include_directories(net PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})
  target_include_directories(netd PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})
  # Ensure the linker can find libraries discovered by pkg-config
  if(LIBYANG_LIBRARY_DIRS)
    target_link_directories(net PRIVATE ${LIBYANG_LIBRARY_DIRS})
    target_link_directories(netd PRIVATE ${LIBYANG_LIBRARY_DIRS})
  endif()
  if(LIBNETCONF2_LIBRARY_DIRS)
    target_link_directories(net PRIVATE ${LIBNETCONF2_LIBRARY_DIRS})
    target_link_directories(netd PRIVATE ${LIBNETCONF2_LIBRARY_DIRS})
  endif()

  target_link_libraries(net PRIVATE ${LIBYANG_LIBRARIES} ${LIBNETCONF2_LIBRARIES})
  target_link_libraries(netd PRIVATE ${LIBYANG_LIBRARIES} ${LIBNETCONF2_LIBRARIES})
endif()

target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)

# Link libedit for interactive command-line editing
find_library(EDIT_LIBRARY NAMES edit REQUIRED)
target_link_libraries(net PRIVATE ${EDIT_LIBRARY})

message(STATUS "Found edit library: ${EDIT_LIBRARY}")

# Link libipsec for ipsec_set_policy / ipsec_get_policylen / ipsec_dump_policy
find_library(IPSEC_LIBRARY NAMES ipsec REQUIRED)
target_link_libraries(net PRIVATE ${IPSEC_LIBRARY})

message(STATUS "Found ipsec library: ${IPSEC_LIBRARY}")

install(TARGETS net DESTINATION bin)
