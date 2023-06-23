#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

cmake_minimum_required(VERSION 3.20)

project(udocs-processor)

set(CMAKE_CXX_STANDARD 17)

if(MSVC OR MSYS OR MINGW)
    set(PLATFORM_WIN64 ON)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_UNIX ON)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(PROTO_GENERATED generated)

find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(stringtoolbox REQUIRED)
find_package(inja REQUIRED)
find_package(md4c REQUIRED)
find_package(base64 REQUIRED)
find_package(Crc32c REQUIRED)
find_package(Poco REQUIRED)
find_package(ftxui REQUIRED)
find_package(CLI11 REQUIRED)
find_package(Boost REQUIRED)
find_package(gRPC REQUIRED)
find_package(termcolor REQUIRED)

if(NOT DEFINED DO_BUILD_TESTS)
    set(DO_BUILD_TESTS ON)
endif()

#installs static libs to lib, and headers to include
if(NOT DEFINED DO_INSTALL_AS_CONAN_PACKAGE)
    set(DO_INSTALL_AS_CONAN_PACKAGE OFF)
endif()

if (NOT DEFINED CONAN_PACKAGE_INSTALL_POSTFIX)
    set(CONAN_PACKAGE_INSTALL_POSTFIX _conan)
endif()

if (NOT DEFINED TESTS_INSTALL_POSTFIX)
    set(TESTS_INSTALL_POSTFIX _tests)
endif()

#for more info on Structure of a CMake C++ project used at Medelför
#see https://issues.internal.medelfor.com/youtrack/articles/KG-A-2

#LIBRARY PART
if (PLATFORM_WIN64 AND NOT SKIP_CLI)
    add_subdirectory(module_cli)
endif ()
add_subdirectory(module_processor)
add_subdirectory(module_serializer)
add_subdirectory(module_base)
add_subdirectory(module_report)
add_subdirectory(module_telemetry)
add_subdirectory(module_util)
add_subdirectory(module_cloud)

install(DIRECTORY ${CMAKE_BINARY_DIR}/deps/
        DESTINATION ${CMAKE_INSTALL_PREFIX})

if(DO_BUILD_EXECUTABLE AND PLATFORM_WIN64)
    add_subdirectory(exec)
endif()

#TESTS EXECUTABLE PART
if(DO_BUILD_TESTS)
    add_subdirectory(tests)
endif()
