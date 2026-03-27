file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/*/*.cpp
)

# Explicitly add stop-group command to ensure it's included
list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/commands/stop-group/stop-group.cpp)

# Create executable with all source files
add_executable(app ${SRC_FILES})

target_include_directories(app
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/libraries
        ${PROJECT_SOURCE_DIR}/configs
)

# Use PROJECT_BINARY_DIR instead of CMAKE_BINARY_DIR for consistency
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)