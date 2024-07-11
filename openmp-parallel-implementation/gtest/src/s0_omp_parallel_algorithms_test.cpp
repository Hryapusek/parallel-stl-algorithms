#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <ranges>
#include <vector>
#include <numeric>

#include "s0_parallel_algorithms_openmp.hpp"

TEST(reduce, VectorOf500ElementsDefaultReduce)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    auto result = openMPI.reduce(arr.begin(), arr.end());
    ASSERT_EQ(result, std::reduce(arr.begin(), arr.end()));
}

TEST(reduce, VectorOf500ElementsInitValuePassed)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    auto result = openMPI.reduce(arr.begin(), arr.end(), initValue);
    ASSERT_EQ(result, initValue + std::reduce(arr.begin(), arr.end()));
}

TEST(findIf, SearchWithLambda)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    constexpr int searchValue = 420;
    auto result = openMPI.find_if(arr.begin(), arr.end(), [](int value){return value == searchValue;});
    ASSERT_NE(result, arr.end());
    ASSERT_EQ(*result, searchValue);
}

TEST(findIf, SearchWithLambdaNonExistValue)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    auto result = openMPI.find_if(arr.begin(), arr.end(), [](int value){return false;});
    ASSERT_EQ(result, arr.end());
}

TEST(countIf, valueLess250Test)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    auto searchLambda = [](int value){return value < 250;};
    auto result = openMPI.count_if(arr.begin(), arr.end(), searchLambda);
    ASSERT_EQ(result, std::count_if(arr.begin(), arr.end(), searchLambda));
}

TEST(countIf, alwaysFalseTest)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    auto searchLambda = [](int value){return false;};
    auto result = openMPI.count_if(arr.begin(), arr.end(), searchLambda);
    ASSERT_EQ(result, 0);
}

TEST(countIf, alwaysTrueTest)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    auto searchLambda = [](int value){return true;};
    auto result = openMPI.count_if(arr.begin(), arr.end(), searchLambda);
    ASSERT_EQ(result, arr.size());
}

TEST(transform, increaseBy500)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    std::vector<int> outputArr;
    std::vector<int> expectedArr;
    auto increaseBy500 = [](auto value){return value+500;};
    std::transform(arr.begin(), arr.end(), std::back_inserter(expectedArr), increaseBy500);
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    openMPI.transform(arr.begin(), arr.end(), std::back_inserter(outputArr), increaseBy500);
    auto result = std::equal(outputArr.begin(), outputArr.end(), expectedArr.begin(), expectedArr.end());
    ASSERT_TRUE(result);
}

TEST(transformNonBackInserter, increaseBy500)
{
    auto range = std::ranges::views::iota(0, 500);
    std::vector<int> arr(range.begin(), range.end());
    std::vector<int> outputArr(arr.size(), 0);
    std::vector<int> expectedArr;
    auto increaseBy500 = [](auto value){return value+500;};
    std::transform(arr.begin(), arr.end(), std::back_inserter(expectedArr), increaseBy500);
    s0m4b0dY::OpenMP openMPI;
    int initValue = 200;
    openMPI.transform_non_back_inserter(arr.begin(), arr.end(), outputArr.begin(), increaseBy500);
    auto result = std::equal(outputArr.begin(), outputArr.end(), expectedArr.begin(), expectedArr.end());
    ASSERT_TRUE(result);
}

TEST(transformSecondOverload_adder, plus)
{
    auto range1 = std::ranges::views::iota(0, 500);
    auto range2 = std::ranges::views::iota(500, 1000);
    std::vector<int> arr1(range1.begin(), range1.end());
    std::vector<int> arr2(range2.begin(), range2.end());
    std::vector<int> outputArr;
    std::vector<int> expectedArr;
    std::transform(arr1.begin(), arr1.end(), arr2.begin(), std::back_inserter(expectedArr), std::plus());
    s0m4b0dY::OpenMP openMPI;
    openMPI.transform(arr1.begin(), arr1.end(), arr2.begin(), std::back_inserter(outputArr), std::plus());
    auto result = std::equal(outputArr.begin(), outputArr.end(), outputArr.begin(), outputArr.end());
    ASSERT_TRUE(result);
}

TEST(transformSecondOverloadNonBackInserter, plus)
{
    auto range1 = std::ranges::views::iota(0, 500);
    auto range2 = std::ranges::views::iota(500, 1000);
    std::vector<int> arr1(range1.begin(), range1.end());
    std::vector<int> arr2(range2.begin(), range2.end());
    std::vector<int> outputArr(arr1.size());
    std::vector<int> expectedArr;
    std::transform(arr1.begin(), arr1.end(), arr2.begin(), std::back_inserter(expectedArr), std::plus());
    s0m4b0dY::OpenMP openMPI;
    openMPI.transform_non_back_inserter(arr1.begin(), arr1.end(), arr2.begin(), outputArr.begin(), std::plus());
    auto result = std::equal(outputArr.begin(), outputArr.end(), outputArr.begin(), outputArr.end());
    ASSERT_TRUE(result);
}

TEST(bitonicSort, 500_0_range)
{
    std::vector<int> arr;
    for (auto i = 0; i < 512; i++)
    {
        arr.push_back(i);
    }
    std::reverse(arr.begin(), arr.end());
    s0m4b0dY::OpenMP openMPI;
    openMPI.bitonic_sort(arr.begin(), arr.end());
    ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(oddEvenSort, 500_0_range)
{
    std::vector<int> arr;
    for (auto i = 0; i < 511; i++)
    {
        arr.push_back(i);
    }
    std::reverse(arr.begin(), arr.end());
    s0m4b0dY::OpenMP openMPI;
    openMPI.odd_even_sort(arr.begin(), arr.end());
    ASSERT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

class SortPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize random data
        for (int i = 0; i < testDataSize; ++i) {
            data.push_back(rand() % 10000);
        }
    }

    static constexpr size_t testDataSize = 1<<16;
    std::vector<int> data;
};

TEST_F(SortPerformanceTest, SequentialSortPerformance) {
    std::vector<int> dataCopy = data;

    auto start = std::chrono::high_resolution_clock::now();
    std::sort(dataCopy.begin(), dataCopy.end());
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sequential sort time: " << duration << " ms" << std::endl;

    // Check if sorted
    ASSERT_TRUE(std::is_sorted(dataCopy.begin(), dataCopy.end()));
}

TEST_F(SortPerformanceTest, BitonicSortPerformance) {
    std::vector<int> dataCopy = data;

    s0m4b0dY::OpenMP threading;

    auto start = std::chrono::high_resolution_clock::now();
    threading.bitonic_sort(dataCopy.begin(), dataCopy.end());
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Bitonic sort time: " << duration << " ms" << std::endl;

    // Check if sorted
    ASSERT_TRUE(std::is_sorted(dataCopy.begin(), dataCopy.end()));
}

TEST_F(SortPerformanceTest, OddEvenSortPerformance) {
    std::vector<int> dataCopy = data;

    s0m4b0dY::OpenMP threading;

    auto start = std::chrono::high_resolution_clock::now();
    threading.odd_even_sort(dataCopy.begin(), dataCopy.end());
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Odd-Even sort time: " << duration << " ms" << std::endl;

    // Check if sorted
    ASSERT_TRUE(std::is_sorted(dataCopy.begin(), dataCopy.end()));
}
