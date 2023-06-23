#
#  Copyright © 2022, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

project(udocs-processor_module_telemetry)

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
            api/telemetry/Telemetry.proto
)

add_library(${PROJECT_NAME} SHARED
        generated/api/Status.pb.h
        generated/api/Status.pb.cc
        generated/api/telemetry/Telemetry.grpc.pb.cc
        generated/api/telemetry/Telemetry.grpc.pb.h
        generated/api/telemetry/Telemetry.pb.cc
        generated/api/telemetry/Telemetry.pb.h
        telemetry/TelemetryReporter.cpp
        include/udocs-processor/telemetry/TelemetryReporter.h
        include/udocs-processor/telemetry/TelemetryService.h
        telemetry/grpcTelemetryService.cpp
        include/udocs-processor/telemetry/grpcTelemetryService.h
        telemetry/BasicTelemetry.cpp
        include/udocs-processor/telemetry/BasicTelemetry.h
        include/udocs-processor/telemetry/grpc.h
        telemetry/grpc.cpp
        telemetry/LogReporter.cpp
        include/udocs-processor/telemetry/LogReporter.h)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "M-UDocs-Telemetry")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/")

set(UNIX_LIBRARIES "")

if(PLATFORM_UNIX)
    set(UNIX_LIBRARIES pthread)
endif()

target_link_libraries(${PROJECT_NAME}
        ${UNIX_LIBRARIES}
        udocs-processor_module_base
        gRPC::grpc++
        udocs-processor_module_processor
        nlohmann_json::nlohmann_json
        spdlog::spdlog)

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
