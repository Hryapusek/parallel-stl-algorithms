set(test_target "s0m4b0dY_parallel_algorithms_binary_test")

include(cmake/install_gtest.cmake)

install_gtest()

set(TEST_EXECUTABLE_SOURCES
    src/main.cpp
    ${SOURCE_FILES}
)
set(TEST_EXECUTABLE_INCLUDES_PUBLIC
    ${COMMON_PUBLIC_INCLUDES}
)

add_executable(${test_target} ${SOURCE_FILES})

target_sources(${test_target}
    PRIVATE ${TEST_EXECUTABLE_SOURCES}
    )

target_include_directories(${test_target}
    PUBLIC ${TEST_EXECUTABLE_INCLUDES_PUBLIC}
    PRIVATE ${COMMON_PRIVATE_INCLUDES}
)

target_link_libraries(${test_target} gtest gtest_main)

target_compile_features(${test_target} PUBLIC cxx_std_20)
