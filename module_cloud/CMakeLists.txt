#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

project(udocs-processor_module_cloud)

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
            surapi/Document.proto
            surapi/Organization.proto
            surapi/Project.proto
            surapi/Quota.proto
            surapi/Token.proto
)

add_library(${PROJECT_NAME} SHARED
        generated/api/Status.pb.h
        generated/api/Status.pb.cc
        generated/surapi/Document.grpc.pb.cc
        generated/surapi/Document.grpc.pb.h
        generated/surapi/Document.pb.cc
        generated/surapi/Document.pb.h
        generated/surapi/Organization.grpc.pb.cc
        generated/surapi/Organization.grpc.pb.h
        generated/surapi/Organization.pb.cc
        generated/surapi/Organization.pb.h
        generated/surapi/Project.grpc.pb.cc
        generated/surapi/Project.grpc.pb.h
        generated/surapi/Project.pb.cc
        generated/surapi/Project.pb.h
        generated/surapi/Quota.grpc.pb.cc
        generated/surapi/Quota.grpc.pb.h
        generated/surapi/Quota.pb.cc
        generated/surapi/Quota.pb.h
        generated/surapi/Token.grpc.pb.cc
        generated/surapi/Token.grpc.pb.h
        generated/surapi/Token.pb.cc
        generated/surapi/Token.pb.h
        include/udocs-processor/services/TokenService.h
        include/udocs-processor/services/QuotaService.h
        include/udocs-processor/services/ProjectService.h
        include/udocs-processor/services/DocumentService.h
        include/udocs-processor/services/OrganizationService.h
        grpc/grpcHelper.cpp
        include/udocs-processor/grpc/grpcHelper.h
        include/udocs-processor/services/impl/TokenServiceImpl.h
        include/udocs-processor/services/impl/DocumentServiceImpl.h
        include/udocs-processor/services/impl/QuotaServiceImpl.h
        include/udocs-processor/services/impl/OrganizationServiceImpl.h
        include/udocs-processor/services/impl/ProjectServiceImpl.h
        services/impl/OrganizationServiceImpl.cpp
        services/impl/ProjectServiceImpl.cpp
        services/impl/DocumentServiceImpl.cpp
        services/impl/TokenServiceImpl.cpp
        services/impl/QuotaServiceImpl.cpp
        include/udocs-processor/services/DocumentServiceFactory.h
        services/impl/DocumentServiceFactoryImpl.cpp
        include/udocs-processor/services/impl/DocumentServiceFactoryImpl.h
        )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "M-UDocs-Cloud")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

set(UNIX_LIBRARIES "")

if(PLATFORM_UNIX)
    set(UNIX_LIBRARIES pthread)
endif()

target_link_libraries(${PROJECT_NAME}
        ${UNIX_LIBRARIES}
        gRPC::grpc++
        udocs-processor_module_processor
        udocs-processor_module_serializer
        udocs-processor_module_util)

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
