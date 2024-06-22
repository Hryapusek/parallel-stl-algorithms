cmake_minimum_required(VERSION 3.22)

project(s0m4b0dY_parallel_algorithms)


set(GTEST_EXECUTABLE ON CACHE BOOL "Build gtest executable?")

set(main_lib "s0m4b0dY_parallel_algorithms")

set(SOURCE_FILES
)

set(COMMON_PUBLIC_INCLUDES
    include/public
)

if (GTEST_EXECUTABLE)
    include(cmake/build_test_executable.cmake)
endif()

add_library(${main_lib} INTERFACE ${COMMON_PUBLIC_INCLUDES})

# target_include_directories(${main_lib}
#     PUBLIC include/public
# )

# target_sources(${main_lib} # Header only?
#     PUBLIC ${SOURCE_FILES}
# )

target_compile_features(${main_lib} INTERFACE cxx_std_20)


