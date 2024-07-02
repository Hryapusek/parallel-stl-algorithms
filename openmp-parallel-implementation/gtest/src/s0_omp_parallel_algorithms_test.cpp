#include "gtest/gtest.h"

#include <ranges>
#include <vector>
#include <numeric>

#include "s0_parallel_algorithms_openmp.hpp"

TEST(reduceParallelAlgorithm, VectorOf500ElementsDefaultReduce)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMPI openMPI;
    auto result = openMPI.reduce(arr.begin(), arr.end());
    ASSERT_EQ(result, std::reduce(arr.begin(), arr.end()));
}

TEST(reduceParallelAlgorithm, VectorOf500ElementsInitValuePassed)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMPI openMPI;
    int initValue = 200;
    auto result = openMPI.reduce(arr.begin(), arr.end(), initValue);
    ASSERT_EQ(result, initValue + std::reduce(arr.begin(), arr.end()));
}
