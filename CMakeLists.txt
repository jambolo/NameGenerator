cmake_minimum_required(VERSION 3.21)
project(NameGenerator 
    VERSION 0.1.0 
    DESCRIPTION "A name generator application using RandomWordGenerator library"
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
# Executable                                                            #
#########################################################################

# Main executable target
add_executable(generate_name)
target_sources(generate_name
    PRIVATE
        main.cpp
)

# Modern target-based approach
target_include_directories(generate_name 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(generate_name 
    PRIVATE
        RandomWordGenerator::RandomWordGenerator
        CLI11::CLI11
)

# Apply the same compile features as the library
target_compile_features(generate_name PRIVATE cxx_std_17)

# IDE source grouping
source_group("Source Files" FILES main.cpp)


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
