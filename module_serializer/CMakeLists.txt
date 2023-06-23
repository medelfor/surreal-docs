#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

project(udocs-processor_module_serializer)

add_library(${PROJECT_NAME} SHARED
        include/udocs-processor/serializers/DocTreeSerializer.h
        serializers/HTMLDocTreeSerializer.cpp
        include/udocs-processor/serializers/HTMLDocTreeSerializer.h
        serializers/UFunctionHTMLSerializer.cpp
        include/udocs-processor/serializers/UFunctionHTMLSerializer.h
        include/udocs-processor/serializers/UFunctionSerializer.h
        serializers/DocTreeSerializer.cpp
        include/udocs-processor/serializers/type/UTypeHTMLSerializer.h
        include/udocs-processor/serializers/type/UTypeSerializer.h
        serializers/type/UTypeHTMLSerializer.cpp
        serializers/JSONDocTreeSerializer.cpp
        include/udocs-processor/serializers/JSONDocTreeSerializer.h
        include/udocs-processor/serializers/DynamicPageData.h
        serializers/DynamicPageData.cpp
        include/udocs-processor/LoggerRegistrar.h
        LoggerRegistrar.cpp
        serializers/markdown/SurrealMarkdownToHtmlConverter.cpp
        include/udocs-processor/serializers/markdown/SurrealMarkdownToHtmlConverter.h
        include/udocs-processor/serializers/markdown/SurrealMarkdownParser.h
        serializers/markdown/SurrealMarkdownParser.cpp
        serializers/markdown/SurrealMarkdown.cpp
        include/udocs-processor/serializers/markdown/SurrealMarkdown.h
        include/udocs-processor/serializers/markdown/SurrealMarkdownConverter.h
        serializers/util/IsLinkInternalDecider.cpp
        include/udocs-processor/serializers/util/IsLinkInternalDecider.h
        include/udocs-processor/serializers/type/UTypeMarkdownSerializer.h
        serializers/type/UTypeMarkdownSerializer.cpp
        serializers/type/UTypeToImagesSerializer.cpp
        include/udocs-processor/serializers/type/UTypeToImagesSerializer.h
        include/udocs-processor/serializers/type/UTypeToStringSerializer.h
        serializers/type/UTypeToStringSerializer.cpp
        serializers/UFunctionSerializer.cpp
        serializers/IntegrityCheck.cpp
        include/udocs-processor/serializers/IntegrityCheck.h
        )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "M-UDocs-Serializer")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

set(UNIX_LIBRARIES "")

if(PLATFORM_UNIX)
    set(UNIX_LIBRARIES pthread)
endif()

target_link_libraries(${PROJECT_NAME}
        ${UNIX_LIBRARIES}
        udocs-processor_module_base
        udocs-processor_module_report
        udocs-processor_module_util
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        stringtoolbox::stringtoolbox
        pantor::inja
        base64::base64
        md4c::md4c
        Crc32c::Crc32c
        Boost::boost
        Boost::filesystem)

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
