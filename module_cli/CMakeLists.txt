#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

project(udocs-processor_module_cli)

set(MODULE_API_PROTO_DIR "${CMAKE_BINARY_DIR}/${RESOURCES_DIR}/res/misc/proto")
file(MAKE_DIRECTORY ${PROTO_GENERATED})
if(PLATFORM_WIN64)
    set(EXTENSION ".exe")
else()
    set(EXTENSION "")
endif()

execute_process(
        COMMAND "${CMAKE_BINARY_DIR}/execs/protoc${EXTENSION}"
            -I ${MODULE_API_PROTO_DIR}
            --plugin=protoc-gen-grpc=${CMAKE_BINARY_DIR}/execs/grpc_cpp_plugin${EXTENSION}
            --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_GENERATED}
            --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_GENERATED}
            api/Status.proto
            api/products/Product.proto
)

add_library(${PROJECT_NAME} SHARED
        generated/api/Status.pb.h
        generated/api/Status.pb.cc
        generated/api/products/Product.grpc.pb.cc
        generated/api/products/Product.grpc.pb.h
        generated/api/products/Product.pb.cc
        generated/api/products/Product.pb.h
        include/udocs-processor/cli/SurDocsCLI.h
        cli/SurDocsCLI.cpp
        include/udocs-processor/cli/views/GenerateView.h
        include/udocs-processor/cli/views/InitView.h
        cli/views/FtxGenerateView.cpp
        include/udocs-processor/cli/views/FtxGenerateView.h
        cli/views/FtxInitView.cpp
        include/udocs-processor/cli/views/FtxInitView.h
        include/udocs-processor/cli/views/View.h
        cli/ue/UnrealInteraction.cpp
        include/udocs-processor/cli/ue/UnrealInteraction.h
        cli/commands/InitCommand.cpp
        include/udocs-processor/cli/commands/InitCommand.h
        cli/commands/GenerateCommand.cpp
        include/udocs-processor/cli/commands/GenerateCommand.h
        cli/commands/generate/CppPreparer.cpp
        include/udocs-processor/cli/commands/generate/CppPreparer.h
        cli/commands/generate/BpPreparer.cpp
        include/udocs-processor/cli/commands/generate/BpPreparer.h
        cli/commands/generate/Generator.cpp
        include/udocs-processor/cli/commands/generate/Generator.h
        cli/SurDocsCLI.cpp
        include/udocs-processor/cli/SurDocsCLI.h
        include/udocs-processor/cli/thirdparty/WinReg.hpp
        include/udocs-processor/cli/commands/generate/rendering/Templater.h
        include/udocs-processor/cli/commands/generate/rendering/UDocsDocumentRenderer.h
        cli/commands/generate/rendering/UDocsDocumentRenderer.cpp
        cli/commands/generate/rendering/Templater.cpp
        cli/cli/GenerateCLI.cpp
        include/udocs-processor/cli/cli/GenerateCLI.h
        cli/views/View.cpp
        cli/views/GenerateView.cpp
        cli/views/FtxView.cpp
        include/udocs-processor/cli/views/FtxView.h
        cli/CLISignalHandler.cpp
        include/udocs-processor/cli/CLISignalHandler.h
        cli/views/NonInteractiveGenerateView.cpp
        include/udocs-processor/cli/views/NonInteractiveGenerateView.h
        cli/surreal/SurrealLoader.cpp
        include/udocs-processor/cli/surreal/SurrealLoader.h
        cli/cli/InitCLI.cpp
        cli/version/VersionChecker.cpp
        cli/version/grpc.cpp
        cli/version/grpcProductService.cpp
        include/udocs-processor/cli/version/ProductService.h
        include/udocs-processor/cli/version/grpc.h
        include/udocs-processor/cli/version/grpcProductService.h
        include/udocs-processor/cli/version/VersionChecker.h
        cli/commands/AuthCommand.cpp
        cli/commands/OrganizationCreateCommand.cpp
        cli/commands/OrganizationDeleteCommand.cpp
        cli/commands/OrganizationListCommand.cpp
        cli/commands/ProjectAnnounceCommand.cpp
        cli/commands/ProjectCollaboratorAddCommand.cpp
        cli/commands/ProjectCollaboratorDeleteCommand.cpp
        cli/commands/ProjectCollaboratorListCommand.cpp
        cli/commands/ProjectCreateCommand.cpp
        cli/commands/ProjectDeleteCommand.cpp
        cli/commands/ProjectListCommand.cpp
        cli/commands/ProjectPublishCommand.cpp
        cli/commands/TokenCreateCommand.cpp
        cli/commands/TokenListCommand.cpp
        cli/commands/QuotaListCommand.cpp
        include/udocs-processor/cli/commands/TokenCreateCommand.h
        include/udocs-processor/cli/commands/TokenListCommand.h
        include/udocs-processor/cli/commands/QuotaListCommand.h
        include/udocs-processor/cli/commands/ProjectPublishCommand.h
        include/udocs-processor/cli/commands/ProjectListCommand.h
        include/udocs-processor/cli/commands/ProjectDeleteCommand.h
        include/udocs-processor/cli/commands/ProjectCreateCommand.h
        include/udocs-processor/cli/commands/ProjectCollaboratorListCommand.h
        include/udocs-processor/cli/commands/ProjectCollaboratorDeleteCommand.h
        include/udocs-processor/cli/commands/ProjectCollaboratorAddCommand.h
        include/udocs-processor/cli/commands/ProjectAnnounceCommand.h
        include/udocs-processor/cli/commands/OrganizationListCommand.h
        include/udocs-processor/cli/commands/OrganizationDeleteCommand.h
        include/udocs-processor/cli/commands/OrganizationCreateCommand.h
        include/udocs-processor/cli/commands/AuthCommand.h
        cli/views/FtxAuthView.cpp
        cli/views/FtxOrganizationCreateView.cpp
        cli/views/FtxOrganizationDeleteView.cpp
        cli/views/FtxOrganizationListView.cpp
        cli/views/FtxProjectAnnounceView.cpp
        cli/views/FtxProjectCollaboratorAddView.cpp
        cli/views/FtxProjectCollaboratorDeleteView.cpp
        cli/views/FtxProjectCollaboratorListView.cpp
        cli/views/FtxProjectCreateView.cpp
        cli/views/FtxProjectDeleteView.cpp
        cli/views/FtxProjectListView.cpp
        cli/views/FtxProjectPublishView.cpp
        cli/views/FtxQuotaListView.cpp
        cli/views/FtxSimpleView.cpp
        cli/views/FtxTokenCreateView.cpp
        cli/views/FtxTokenListView.cpp
        include/udocs-processor/cli/views/FtxAuthView.h
        include/udocs-processor/cli/views/FtxProjectDeleteView.h
        include/udocs-processor/cli/views/FtxProjectListView.h
        include/udocs-processor/cli/views/FtxProjectPublishView.h
        include/udocs-processor/cli/views/FtxQuotaListView.h
        include/udocs-processor/cli/views/FtxSimpleView.h
        include/udocs-processor/cli/views/FtxTokenCreateView.h
        include/udocs-processor/cli/views/FtxTokenListView.h
        include/udocs-processor/cli/views/FtxProjectCreateView.h
        include/udocs-processor/cli/views/FtxProjectCollaboratorListView.h
        include/udocs-processor/cli/views/FtxProjectCollaboratorDeleteView.h
        include/udocs-processor/cli/views/FtxProjectCollaboratorAddView.h
        include/udocs-processor/cli/views/FtxProjectAnnounceView.h
        include/udocs-processor/cli/views/FtxQuotaListView.h
        include/udocs-processor/cli/views/FtxOrganizationCreateView.h
        include/udocs-processor/cli/views/FtxOrganizationDeleteView.h
        include/udocs-processor/cli/views/FtxOrganizationListView.h
        include/udocs-processor/cli/views/OrganizationCreateView.h
        include/udocs-processor/cli/views/OrganizationDeleteView.h
        include/udocs-processor/cli/views/OrganizationListView.h
        include/udocs-processor/cli/views/ProjectAnnounceView.h
        include/udocs-processor/cli/views/ProjectCollaboratorAddView.h
        include/udocs-processor/cli/views/ProjectCollaboratorDeleteView.h
        include/udocs-processor/cli/views/ProjectCollaboratorListView.h
        include/udocs-processor/cli/views/ProjectCreateView.h
        include/udocs-processor/cli/views/ProjectDeleteView.h
        include/udocs-processor/cli/views/ProjectListView.h
        include/udocs-processor/cli/views/ProjectPublishView.h
        include/udocs-processor/cli/views/AuthView.h
        include/udocs-processor/cli/views/TokenCreateView.h
        include/udocs-processor/cli/views/TokenListView.h
        include/udocs-processor/cli/views/QuotaListView.h
        include/udocs-processor/cli/views/SimpleView.h
        cli/cli/OrganizationCreateCLI.cpp
        cli/cli/OrganizationDeleteCLI.cpp
        cli/cli/OrganizationListCLI.cpp
        cli/cli/ProjectAnnounceCLI.cpp
        cli/cli/ProjectCollaboratorAddCLI.cpp
        cli/cli/ProjectCollaboratorDeleteCLI.cpp
        cli/cli/ProjectCollaboratorListCLI.cpp
        cli/cli/ProjectCreateCLI.cpp
        cli/cli/ProjectDeleteCLI.cpp
        cli/cli/ProjectListCLI.cpp
        cli/cli/ProjectPublishCLI.cpp
        cli/cli/QuotaListCLI.cpp
        cli/cli/TokenCreateCLI.cpp
        cli/cli/TokenListCLI.cpp
        cli/cli/AuthCLI.cpp
        include/udocs-processor/cli/cli/AuthCLI.h
        include/udocs-processor/cli/cli/OrganizationCreateCLI.h
        include/udocs-processor/cli/cli/OrganizationDeleteCLI.h
        include/udocs-processor/cli/cli/OrganizationListCLI.h
        include/udocs-processor/cli/cli/ProjectAnnounceCLI.h
        include/udocs-processor/cli/cli/ProjectCollaboratorAddCLI.h
        include/udocs-processor/cli/cli/ProjectCollaboratorDeleteCLI.h
        include/udocs-processor/cli/cli/ProjectCollaboratorListCLI.h
        include/udocs-processor/cli/cli/ProjectCreateCLI.h
        include/udocs-processor/cli/cli/ProjectDeleteCLI.h
        include/udocs-processor/cli/cli/ProjectListCLI.h
        include/udocs-processor/cli/cli/ProjectPublishCLI.h
        include/udocs-processor/cli/cli/QuotaListCLI.h
        include/udocs-processor/cli/cli/TokenCreateCLI.h
        include/udocs-processor/cli/cli/TokenListCLI.h
        )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "M-UDocs-CLI")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

set(UNIX_LIBRARIES "")

if(PLATFORM_UNIX)
    set(UNIX_LIBRARIES pthread)
endif()

target_link_libraries(${PROJECT_NAME}
        ${UNIX_LIBRARIES}
        udocs-processor_module_telemetry
        udocs-processor_module_cloud
        udocs-processor_module_processor
        udocs-processor_module_serializer
        udocs-processor_module_util
        CLI11::CLI11
        Boost::boost
        termcolor::termcolor
        Boost::filesystem
        ftxui::ftxui)

target_include_directories(${PROJECT_NAME} PUBLIC include PUBLIC "generated")
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
