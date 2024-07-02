cmake_minimum_required(VERSION 3.22)

project(s0m4b0dY_parallel_algorithms)

set(GTEST_EXECUTABLE ON CACHE BOOL "Build gtest executable?")

get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
    set(s0m4b0dY_utils_lib "s0m4b0dY_utils" PARENT_SCOPE)
    message(STATUS "Has a parent scope.")
endif()
set(s0m4b0dY_utils_lib "s0m4b0dY_utils")


set(COMMON_SOURCE_FILES
    src/s0_utils.cpp
)

set(COMMON_PUBLIC_INCLUDES
    include/public
)

set(COMMON_PRIVATE_INCLUDES
)

if (GTEST_EXECUTABLE)
    include(cmake/build_test_executable.cmake)
endif()

add_library(${s0m4b0dY_utils_lib})

target_include_directories(${s0m4b0dY_utils_lib}
    PUBLIC ${COMMON_PUBLIC_INCLUDES}
    PRIVATE ${COMMON_PRIVATE_INCLUDES}
)

target_sources(${s0m4b0dY_utils_lib}
    PUBLIC ${COMMON_SOURCE_FILES}
)

target_compile_features(${s0m4b0dY_utils_lib} PUBLIC cxx_std_20)


