# NameGenerator Project

cmake_minimum_required(VERSION 3.21)
project(NameGenerator 
    VERSION 0.3.0 
    DESCRIPTION "Words generator applications using RandomWordGenerator library"
    LANGUAGES CXX
)

# Modern CMake policy settings
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0135 NEW)  # DOWNLOAD_EXTRACT_TIMESTAMP option

# Feature requirements
include(CMakeDependentOption)
include(GNUInstallDirs)

# Build options
option(BUILD_SHARED_LIBS "Build libraries as DLLs" OFF)

# Only enable testing and packaging when this is the main project
set(IS_TOP_LEVEL_PROJECT OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOP_LEVEL_PROJECT ON)
endif()

cmake_dependent_option(NAMEGEN_BUILD_TESTS 
    "Build and run tests" 
    ON 
    "IS_TOP_LEVEL_PROJECT" 
    OFF
)

# C++ standard requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        VC_EXTRALEAN
        _CRT_SECURE_NO_WARNINGS
        _SECURE_SCL=0
        _SCL_SECURE_NO_WARNINGS
    )
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#########################################################################
# Dependencies                                                          #
#########################################################################

include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.2
)
FetchContent_MakeAvailable(CLI11)

add_subdirectory(RandomWordGenerator)

#########################################################################
# Executables                                                           #
#########################################################################

# Add subdirectories for each executable
add_subdirectory(apps/generate_names)

#########################################################################
# Testing                                                               #
#########################################################################

if(NAMEGEN_BUILD_TESTS)
    include(CTest)
    enable_testing()
    message(STATUS "${PROJECT_NAME}: Testing is enabled")
    
    # Enable testing for the RandomWordGenerator library
    set(RANDOMWORDGEN_BUILD_TESTS ON CACHE BOOL "" FORCE)
endif()
