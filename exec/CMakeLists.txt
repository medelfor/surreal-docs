project(exec)

add_executable(exec main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "surdocs")

target_link_libraries(exec
        Poco::NetSSL
        udocs-processor_module_processor
        udocs-processor_module_serializer
        udocs-processor_module_util
        udocs-processor_module_cli
        )

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
