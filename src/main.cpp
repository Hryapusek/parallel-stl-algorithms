#include "parallel_algorithms_openmp.hpp"
#include <gtest/gtest.h>

int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

TEST(Basic, usage)
{
    std::vector<int> vec{1, 2, 3};
    s0m4b0dY::OpenMPI().reduce(vec.begin(), vec.end());
}
