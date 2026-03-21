cmake_minimum_required(VERSION 3.20)

project(app VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands (for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

target_include_directories(app PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_custom_target(run
    COMMAND $<TARGET_FILE:app>
    DEPENDS app
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

