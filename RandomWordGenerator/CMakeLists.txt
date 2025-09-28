# RandomWordGenerator Library

cmake_minimum_required(VERSION 3.21)
project(RandomWordGenerator 
    VERSION 0.1.0 
    DESCRIPTION "A library for generating random words using distribution analysis of a dictionary"
    LANGUAGES CXX
)

# Modern CMake policy settings
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
cmake_policy(SET CMP0135 NEW)  # DOWNLOAD_EXTRACT_TIMESTAMP option

# Feature requirements
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build options
option(BUILD_SHARED_LIBS "Build libraries as DLLs" OFF)

# Only enable certain features when this is the main project
set(IS_TOP_LEVEL_PROJECT OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOP_LEVEL_PROJECT ON)
endif()

cmake_dependent_option(RANDOMWORDGEN_BUILD_TESTS 
    "Build and run RandomWordGenerator tests" 
    ON 
    "IS_TOP_LEVEL_PROJECT" 
    OFF
)

cmake_dependent_option(RANDOMWORDGEN_BUILD_DOCS 
    "Build RandomWordGenerator documentation with Doxygen" 
    OFF 
    "IS_TOP_LEVEL_PROJECT" 
    OFF
)

#########################################################################
# Dependencies                                                          #
#########################################################################

#########################################################################
# Main Library Target                                                   #
#########################################################################

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Modern target_sources approach
target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/RandomWordGenerator/Generator.h
    PRIVATE
        Generator.cpp
)

# Target properties
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} 
    PROPERTIES 
        CXX_EXTENSIONS OFF
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories using modern generator expressions
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/RandomWordGenerator
)

# Link dependencies

# Compiler-specific definitions
if(MSVC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            VC_EXTRALEAN
            _CRT_SECURE_NO_WARNINGS
            _SECURE_SCL=0
            _SCL_SECURE_NO_WARNINGS
    )
endif()

# IDE source grouping
source_group("Header Files" 
    FILES 
        include/RandomWordGenerator/Generator.h
        include/RandomWordGenerator/Factory.h
)
source_group("Source Files" 
    FILES 
        Generator.cpp 
        Factory.cpp
)

#########################################################################
# Documentation                                                         #
#########################################################################

if(RANDOMWORDGEN_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
        set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
        set(DOXYGEN_PROJECT_VERSION ${PROJECT_VERSION})
        set(DOXYGEN_PROJECT_BRIEF "${PROJECT_DESCRIPTION}")
        
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
                      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
                      @ONLY)
        
        add_custom_target(${PROJECT_NAME}_docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating ${PROJECT_NAME} API documentation with Doxygen"
            VERBATIM
            SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        )
        
        # Add to all target only if this is the main project
        if(IS_TOP_LEVEL_PROJECT)
            add_custom_target(docs ALL DEPENDS ${PROJECT_NAME}_docs)
        endif()
    else()
        message(WARNING "Doxygen not found. Documentation will not be generated.")
    endif()
endif()

#########################################################################
# Testing                                                               #
#########################################################################

if(RANDOMWORDGEN_BUILD_TESTS)
    include(CTest)
    enable_testing()
    message(STATUS "${PROJECT_NAME}: Testing is enabled")
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
        add_subdirectory(test)
    else()
        message(WARNING "${PROJECT_NAME}: test directory not found")
    endif()
endif()

#########################################################################
# Installation                                                          #
#########################################################################

# Configure installation paths
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Install the library and headers
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the targets file
install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Create and install package configuration files
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion  # More strict than AnyNewerVersion
)

# Check if config template exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
    )
else()
    # Create a basic config file if template doesn't exist
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake "
include(CMakeFindDependencyMacro)
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)
")
endif()

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Export from build tree for development
export(EXPORT ${PROJECT_NAME}-targets 
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake 
    NAMESPACE ${PROJECT_NAME}::
)

# Register package in user's package registry (only if top-level)
if(IS_TOP_LEVEL_PROJECT)
    export(PACKAGE ${PROJECT_NAME})
endif()

# Install documentation if built
if(RANDOMWORDGEN_BUILD_DOCS AND DOXYGEN_FOUND)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            OPTIONAL)
endif()
