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

string(TOLOWER ${STELLERI} STELLERI_LOWER)

# Option to build the netd daemon when using netconf backend.
# This allows building only the client if desired, even in netconf mode.
option(BUILD_NETD "Build netd daemon (only applicable in netconf mode)" ON)

# Detect OS and set OS-specific sources
set(OS_SOURCES "")
set(OS_LIBS "")
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  file(GLOB_RECURSE OS_SOURCES src/system/freebsd/*.cpp)
  find_library(IPSEC_LIBRARY NAMES ipsec REQUIRED)
  message(STATUS "Found ipsec library: ${IPSEC_LIBRARY}")
  list(APPEND OS_LIBS ${IPSEC_LIBRARY})
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  set(OS_SOURCES)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
  set(OS_SOURCES)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OS_SOURCES)
else()
  message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT OS_SOURCES)
  message(WARNING "No OS-specific sources found for ${CMAKE_SYSTEM_NAME}")
endif()

# Shared sources (data)
file(GLOB_RECURSE DATA_SOURCES src/data/*.cpp)

# Client-only sources
file(GLOB_RECURSE FORMATTER_SOURCES src/formatter/*.cpp)
file(GLOB_RECURSE PARSER_SOURCES src/parser/*.cpp)
set(CLI_SOURCE src/CLI.cpp)

# Potentially shared execution sources
file(GLOB_RECURSE EXECUTIVE_SOURCES src/executive/*.cpp)
file(GLOB_RECURSE GENERATOR_SOURCES src/generator/*.cpp)

# Netconf sources (client-side)
file(GLOB_RECURSE NETCONF_SOURCES src/system/netconf/*.cpp)

# Shared library sources
set(SHARED_SOURCES ${DATA_SOURCES} ${EXECUTIVE_SOURCES} ${GENERATOR_SOURCES} ${OS_SOURCES})
if(STELLERI_LOWER STREQUAL "netconf")
  list(APPEND SHARED_SOURCES src/server/NetconfExecutor.cpp)
endif()

add_library(stelleri_lib STATIC ${SHARED_SOURCES})
target_include_directories(stelleri_lib PUBLIC include)
target_compile_options(stelleri_lib PRIVATE -Wall -Wextra -Werror -pedantic)
if(STELLERI_LOWER STREQUAL "netconf")
  target_compile_definitions(stelleri_lib PRIVATE STELLERI_NETCONF=1)
endif()

# Sources for the net client
set(NET_SPECIFIC_SOURCES ${FORMATTER_SOURCES} ${PARSER_SOURCES} ${CLI_SOURCE} src/main.cpp)
if(STELLERI_LOWER STREQUAL "netconf")
  list(APPEND NET_SPECIFIC_SOURCES ${NETCONF_SOURCES})
endif()

add_executable(net ${NET_SPECIFIC_SOURCES})
target_include_directories(net PRIVATE include)
target_compile_options(net PRIVATE -Wall -Wextra -Werror -pedantic)
target_link_libraries(net PRIVATE stelleri_lib)

# Link libedit for interactive command-line editing in net.
find_library(EDIT_LIBRARY NAMES edit REQUIRED)
target_link_libraries(net PRIVATE ${EDIT_LIBRARY})
message(STATUS "Found edit library: ${EDIT_LIBRARY}")

# OS-specific links for net
target_link_libraries(net PRIVATE ${OS_LIBS})

# Set compile definition for net if netconf mode
if(STELLERI_LOWER STREQUAL "netconf")
  target_compile_definitions(net PRIVATE STELLERI_NETCONF=1)
endif()

# Handle netconf-specific setup.
if(STELLERI_LOWER STREQUAL "netconf")
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

  # Apply includes to stelleri_lib
  target_include_directories(stelleri_lib PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})

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
    set(NETD_SPECIFIC_SOURCES src/server/Server.cpp src/server/main.cpp)

    add_executable(netd ${NETD_SPECIFIC_SOURCES})
    target_include_directories(netd PRIVATE include)
    target_compile_options(netd PRIVATE -Wall -Wextra -Werror -pedantic)
    target_compile_definitions(netd PRIVATE STELLERI_NETCONF=1)
    target_link_libraries(netd PRIVATE stelleri_lib)

    # Apply same includes and links to netd.
    target_include_directories(netd PRIVATE ${LIBYANG_INCLUDE_DIRS} ${LIBNETCONF2_INCLUDE_DIRS})
    if(LIBYANG_LIBRARY_DIRS)
      target_link_directories(netd PRIVATE ${LIBYANG_LIBRARY_DIRS})
    endif()
    if(LIBNETCONF2_LIBRARY_DIRS)
      target_link_directories(netd PRIVATE ${LIBNETCONF2_LIBRARY_DIRS})
    endif()
    target_link_libraries(netd PRIVATE ${LIBYANG_LIBRARIES} ${LIBNETCONF2_LIBRARIES})

    # OS-specific links for netd
    target_link_libraries(netd PRIVATE ${OS_LIBS})

    install(TARGETS netd DESTINATION bin)
  endif()
endif()

install(TARGETS net DESTINATION bin)

