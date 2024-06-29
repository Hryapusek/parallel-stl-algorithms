#include "gtest/gtest.h"

#include <ranges>
#include <vector>

#include "CommonUtils/s0_utils.hpp"

TEST(generateRanges, defaultValue)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    auto n_cpu = 10;
    auto jobs = 500/n_cpu;
    auto result = generateRanges(arr.begin(), arr.end(), n_cpu);
    ASSERT_EQ(result.size(), n_cpu);
    auto i = 0;
    for (auto pair : result)
    {
        ASSERT_EQ(pair.first.base(), (arr.begin() + i*jobs).base());
        ASSERT_EQ(pair.second.base(), (arr.begin() + (i+1)*jobs).base());
        i++;
    }
}

TEST(generateRanges, oneRange)
{
    auto max_value =  500;
    auto range = std::ranges::views::iota(0, max_value);
    std::vector<int> arr(range.begin(), range.end());
    auto n_cpu = 50;
    auto jobs = max_value/n_cpu;
    auto result = generateRanges(arr.begin(), arr.end(), n_cpu);
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].first, arr.begin());
    ASSERT_EQ(result[0].second, arr.end());
}
