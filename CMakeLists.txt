cmake_minimum_required (VERSION 3.10)
project(NameGenerator VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build libraries as DLLs" FALSE)

#if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#    find_package(nlohmann_json REQUIRED)
#endif()

set(GENERATE_NAME_INCLUDE_PATHS
    .
)

set(GENERATE_NAME_SOURCES
    main.cpp
)
source_group(Sources FILES ${GENERATE_NAME_SOURCES})

add_executable(generate_name ${GENERATE_NAME_SOURCES})
target_include_directories(generate_name PRIVATE ${GENERATE_NAME_INCLUDE_PATHS})
target_link_libraries(generate_name PUBLIC
    RandomWordGenerator
)
target_compile_definitions(generate_name
    PRIVATE
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -DVC_EXTRALEAN
        -D_CRT_SECURE_NO_WARNINGS
        -D_SECURE_SCL=0
        -D_SCL_SECURE_NO_WARNINGS
)

#configure_file("${PROJECT_SOURCE_DIR}/Version.h.in" "${PROJECT_BINARY_DIR}/Version.h")

add_subdirectory(RandomWordGenerator)
