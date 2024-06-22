cmake_minimum_required(VERSION 3.22)

project(s0m4b0dY_parallel_algorithms)


set(GTEST_EXECUTABLE ON CACHE BOOL "Build gtest executable?")

set(main_lib "s0m4b0dY_parallel_algorithms")

set(SOURCE_FILES
    src/placeholder.cpp
)

if (GTEST_EXECUTABLE)
    include(cmake/build_test_executable.cmake)
endif()

add_library(${main_lib} ${SOURCE_FILES})

target_include_directories(${main_lib}
    PUBLIC include
)

# target_sources(${main_lib} # Header only?
#     PUBLIC ${SOURCE_FILES}
# )

target_compile_features(${main_lib} PUBLIC cxx_std_20)


