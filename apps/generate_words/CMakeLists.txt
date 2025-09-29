# generate_words Executable

add_executable(generate_words)
target_sources(generate_words
    PRIVATE
        main.cpp
)

target_include_directories(generate_words 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(generate_words 
    PRIVATE
        RandomWordGenerator::RandomWordGenerator
        CLI11::CLI11
)

target_compile_features(generate_words PRIVATE cxx_std_17)

source_group("Source Files" FILES main.cpp)

set_target_properties(generate_words PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)