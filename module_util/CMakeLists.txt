#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

project(udocs-processor_module_util)

add_library(${PROJECT_NAME} SHARED
        include/udocs-processor/FileDataLoader.h
        FileDataLoader.cpp
        NodesHelper.cpp
        include/udocs-processor/NodesHelper.h
        StringHelper.cpp
        include/udocs-processor/StringHelper.h
        ExecStreamHelper.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "M-UDocs-Util")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

set(UNIX_LIBRARIES "")

if(PLATFORM_UNIX)
    set(UNIX_LIBRARIES pthread)
endif()

target_link_libraries(${PROJECT_NAME}
        ${UNIX_LIBRARIES}
        udocs-processor_module_base
        Boost::boost
        Boost::filesystem
        spdlog::spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE "")

# each DLL should install it to dist and to dist_conan

# dll, so
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})

if(DO_INSTALL_AS_CONAN_PACKAGE)
    set(INSTALL_CONAN_PACKAGE_TO ${CMAKE_INSTALL_PREFIX}${CONAN_PACKAGE_INSTALL_POSTFIX})

    install(TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION ${INSTALL_CONAN_PACKAGE_TO}/lib
            LIBRARY DESTINATION ${INSTALL_CONAN_PACKAGE_TO}/bin
            RUNTIME DESTINATION ${INSTALL_CONAN_PACKAGE_TO}/bin
            )
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            DESTINATION ${INSTALL_CONAN_PACKAGE_TO}/include/)
endif()
