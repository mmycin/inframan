# src/CMakeLists.txt
add_executable(app 
	main.cpp
	hello.cpp
)

target_include_directories(app
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)