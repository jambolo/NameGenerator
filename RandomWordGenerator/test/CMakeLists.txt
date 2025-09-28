cmake_minimum_required(VERSION 3.21)

#########################################################################
# Dependencies                                                          #
#########################################################################

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include Google Test's cmake helper
include(GoogleTest)

#########################################################################
# Test Executable                                                       #
#########################################################################

add_executable(RandomWordGenerator_tests
    test_generator.cpp
    test_analysis.cpp
    test_generation.cpp
    test_edge_cases.cpp
    test_state.cpp
)

target_link_libraries(RandomWordGenerator_tests
    PRIVATE
        RandomWordGenerator::RandomWordGenerator
        gtest_main
        gtest
)

target_compile_features(RandomWordGenerator_tests PRIVATE cxx_std_17)

# Compiler-specific definitions
if(MSVC)
    target_compile_definitions(RandomWordGenerator_tests
        PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            VC_EXTRALEAN
            _CRT_SECURE_NO_WARNINGS
            _SECURE_SCL=0
            _SCL_SECURE_NO_WARNINGS
    )
endif()

# Discover tests for CTest integration
gtest_discover_tests(RandomWordGenerator_tests)
