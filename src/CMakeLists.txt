file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Create executable with all source files
add_executable(app ${SRC_FILES})

target_include_directories(app
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)