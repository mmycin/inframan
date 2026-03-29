cmake_minimum_required(VERSION 3.20)

project(inframan VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

# Export compile commands (for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

target_include_directories(inframan 
	PRIVATE 
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/include/libraries
		${PROJECT_SOURCE_DIR}/configs
)

target_precompile_headers(inframan
    PRIVATE
        ${LIBRARY_HEADERS}
)

# Installation Rules
include(GNUInstallDirs)
install(TARGETS inframan
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CPack Configuration
set(CPACK_PACKAGE_NAME "inframan")
set(CPACK_PACKAGE_VENDOR "Mycin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Infrastructure Manager CLI")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "inframan")

# Generators
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_NSIS_PACKAGE_NAME "inframan")
    set(CPACK_NSIS_DISPLAY_NAME "inframan")
    set(CPACK_NSIS_HELP_LINK "https://github.com/mmycin/inframan")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/mmycin/inframan")
    set(CPACK_NSIS_CONTACT "Mycin")
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mycin")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

add_custom_target(run
    COMMAND $<TARGET_FILE:inframan>
    DEPENDS inframan
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
