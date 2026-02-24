cmake_minimum_required(VERSION 3.25)

# Prefer clang/clang++ if available and no compiler is explicitly set.
# This must be before project() to use the compilers during configuration.
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

# Configuration backend: "lite" (default, uses FreeBSD system calls) or "netconf" (uses NETCONF stub).
set(STELLERI "lite" CACHE STRING "Configuration backend: lite or netconf")
set_property(CACHE STELLERI PROPERTY STRINGS lite netconf)

message(STATUS "Configuration backend: ${STELLERI}")

# Option to build the netd daemon when using netconf backend.
# This allows building only the client if desired, even in netconf mode.
option(BUILD_NETD "Build netd daemon (only applicable in netconf mode)" ON)

file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")
file(GLOB_RECURSE SERVER_SOURCES "src/server/*.cpp")
file(GLOB_RECURSE NETCONF_SOURCES "src/system/netconf/*.cpp")

# In lite mode, exclude netconf and server sources.
if(NOT STELLERI STREQUAL "netconf")
  list(REMOVE_ITEM ALL_SOURCES ${NETCONF_SOURCES} ${SERVER_SOURCES})
endif()

# Sources for the net client: all sources except server sources.
set(NET_SOURCES ${ALL_SOURCES})
list(REMOVE_ITEM NET_SOURCES ${SERVER_SOURCES})

# In netconf mode, the net client needs NetconfExecutor from server sources.
if(STELLERI STREQUAL "netconf")
  list(APPEND NET_SOURCES ${CMAKE_SOURCE_DIR}/src/server/NetconfExecutor.cpp)
endif()

add_executable(net ${NET_SOURCES})
target_include_directories(net PRIVATE include)
target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)

# Link libedit for interactive command-line editing in net.
find_library(EDIT_LIBRARY NAMES edit REQUIRED)
target_link_libraries(net PRIVATE ${EDIT_LIBRARY})
message(STATUS "Found edit library: ${EDIT_LIBRARY}")

# Link libipsec for IPsec policy functions in net.
find_library(IPSEC_LIBRARY NAMES ipsec REQUIRED)
target_link_libraries(net PRIVATE ${IPSEC_LIBRARY})
message(STATUS "Found ipsec library: ${IPSEC_LIBRARY}")

# Handle netconf-specific setup.
if(STELLERI STREQUAL "netconf")
  # Define netconf mode for compilation.
  target_compile_definitions(net PRIVATE STELLERI_NETCONF=1)

  # Use pkg-config to find libyang and libnetconf2.
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBYANG REQUIRED libyang)
  pkg_check_modules(LIBNETCONF2 REQUIRED libnetconf2)

  message(STATUS "libyang include dirs: ${LIBYANG_INCLUDE_DIRS}")
  message(STATUS "libyang library dirs: ${LIBYANG_LIBRARY_DIRS}")
  message(STATUS "libyang libraries: ${LIBYANG_LIBRARIES}")

  message(STATUS "libnetconf2 include dirs: ${LIBNETCONF2_INCLUDE_DIRS}")
  message(STATUS "libnetconf2 library dirs: ${LIBNETCONF2_LIBRARY_DIRS}")
  message(STATUS "libnetconf2 libraries: ${LIBNETCONF2_LIBRARIES}")

  # Apply includes and links to net.
  target_include_directories(net PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})
  if(LIBYANG_LIBRARY_DIRS)
    target_link_directories(net PRIVATE ${LIBYANG_LIBRARY_DIRS})
  endif()
  if(LIBNETCONF2_LIBRARY_DIRS)
    target_link_directories(net PRIVATE ${LIBNETCONF2_LIBRARY_DIRS})
  endif()
  target_link_libraries(net PRIVATE ${LIBYANG_LIBRARIES} ${LIBNETCONF2_LIBRARIES})

  # Build netd only if enabled.
  if(BUILD_NETD)
    add_executable(netd ${SERVER_SOURCES})
    target_include_directories(netd PRIVATE include)
    target_compile_options(netd PRIVATE -Wall -Wextra -Werror -pedantic)
    target_compile_definitions(netd PRIVATE STELLERI_NETCONF=1)

    # Apply same includes and links to netd.
    target_include_directories(netd PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})
    if(LIBYANG_LIBRARY_DIRS)
      target_link_directories(netd PRIVATE ${LIBYANG_LIBRARY_DIRS})
    endif()
    if(LIBNETCONF2_LIBRARY_DIRS)
      target_link_directories(netd PRIVATE ${LIBNETCONF2_LIBRARY_DIRS})
    endif()
    target_link_libraries(netd PRIVATE ${LIBYANG_LIBRARIES} ${LIBNETCONF2_LIBRARIES})

    install(TARGETS netd DESTINATION bin)
  endif()
endif()

install(TARGETS net DESTINATION bin)

