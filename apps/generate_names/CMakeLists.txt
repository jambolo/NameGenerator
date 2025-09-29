# generate_names Executable

add_executable(generate_names)
target_sources(generate_names
    PRIVATE
        main.cpp
)

target_include_directories(generate_names 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(generate_names 
    PRIVATE
        RandomWordGenerator::RandomWordGenerator
        CLI11::CLI11
)

target_compile_features(generate_names PRIVATE cxx_std_17)

source_group("Source Files" FILES main.cpp)

set_target_properties(generate_names PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)