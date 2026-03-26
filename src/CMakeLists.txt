file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Create executable with all source files
add_executable(app ${SRC_FILES})

target_include_directories(app
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

# Use PROJECT_BINARY_DIR instead of CMAKE_BINARY_DIR for consistency
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
)