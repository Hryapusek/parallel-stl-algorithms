cmake_minimum_required(VERSION 3.22)

project(s0m4b0dY_parallel_algorithms)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_subdirectory(../utils-library ./build/utils)

get_target_property(${s0m4b0dY_utils_lib}_INCLUDES ${s0m4b0dY_utils_lib} INCLUDE_DIRECTORIES)


set(GTEST_EXECUTABLE ON CACHE BOOL "Build gtest executable?")

set(main_lib "s0m4b0dY_parallel_algorithms_openmp")

set(SOURCE_FILES
)

set(COMMON_PUBLIC_INCLUDES
    include/public
    ${${s0m4b0dY_utils_lib}_INCLUDES}
)

set(COMMON_PRIVATE_INCLUDES
    include/private
)

set(COMMON_LIBS ${s0m4b0dY_utils_lib})

if (GTEST_EXECUTABLE)
    include(cmake/build_test_executable.cmake)
endif()

add_library(${main_lib} INTERFACE)

target_include_directories(${main_lib}
    INTERFACE ${COMMON_PUBLIC_INCLUDES}
    INTERFACE ${COMMON_PRIVATE_INCLUDES}
)

target_link_libraries(${main_lib} INTERFACE ${COMMON_LIBS})

# target_sources(${main_lib} # Header only?
#     PUBLIC ${SOURCE_FILES}
# )

target_compile_features(${main_lib} INTERFACE cxx_std_20)
