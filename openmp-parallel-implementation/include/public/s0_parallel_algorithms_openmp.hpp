#ifndef S0M4B0D4_PARALLEL_ALGORITHMS
#define S0M4B0D4_PARALLEL_ALGORITHMS

#include <type_traits>
#include <vector>
#include <optional>
#include <atomic>
#include <omp.h>
#include <unordered_map>
#include <execution>

#include "CommonUtils/s0_type_traits.hpp"
#include "CommonUtils/s0_utils.hpp"

namespace s0m4b0dY
{
  class OpenMPI
  {
    template < class T >
    using IteratorValueType = _helpers::IteratorValueType<T>;
  public:
    template <_helpers::AddableIterator Iterator_t>
    IteratorValueType<Iterator_t>::value_type reduce(Iterator_t begin, Iterator_t end);

    template <_helpers::AddableIterator Iterator_t>
    IteratorValueType<Iterator_t>::value_type reduce(Iterator_t begin, Iterator_t end, IteratorValueType<Iterator_t>::value_type initValue);

    template <_helpers::AddableIterator Iterator_t, _helpers::Predicate<typename IteratorValueType<Iterator_t>::value_type> Predicate>
    Iterator_t find_if(Iterator_t begin, Iterator_t end, Predicate &&unaryFunction);

    template <class Iterator_t, _helpers::Predicate<typename IteratorValueType<Iterator_t>::value_type> Predicate>
    long long count_if(Iterator_t begin, Iterator_t end, Predicate &&unaryFunction);

    template <class InputIterator_t, class OutputIterator_t, class UnaryFunction, 
              class = std::enable_if_t<
                          std::is_assignable_v<
                              decltype(*std::declval<OutputIterator_t>()), 
                              decltype(std::declval<UnaryFunction>()(
                                  std::declval<typename IteratorValueType<InputIterator_t>::value_type>()
                                      ))
                                              >
                                      >
              >
    void transform(InputIterator_t begin, InputIterator_t end, OutputIterator_t output, UnaryFunction &&unaryFunction);

    /**
     * @note You should not use it if OutputIterator_t is back_inserter.
     */
    template <class InputIterator_t, class OutputIterator_t, class UnaryFunction, 
              class = std::enable_if_t<
                          std::is_assignable_v<
                              decltype(*std::declval<OutputIterator_t>()), 
                              decltype(std::declval<UnaryFunction>()(
                                  std::declval<typename IteratorValueType<InputIterator_t>::value_type>()
                                      ))
                                              >
                                      >
              >
    void transform_non_back_inserter(InputIterator_t begin, InputIterator_t end, OutputIterator_t output, UnaryFunction &&unaryFunction);
    
    template <class InputIterator1_t, class InputIterator2_t, class OutputIterator_t, class BinaryFunction,
    class = std::enable_if_t<
                          std::is_assignable_v<
                              decltype(*std::declval<OutputIterator_t>()), 
                              decltype(std::declval<BinaryFunction>()(
                                  std::declval<typename IteratorValueType<InputIterator1_t>::value_type>(),
                                  std::declval<typename IteratorValueType<InputIterator2_t>::value_type>()
                                      ))
                                              >
                                      >
              >
    void transform(InputIterator1_t begin1, InputIterator1_t end1, InputIterator2_t begin2, OutputIterator_t output, BinaryFunction &&binaryFunction);

    /**
     * @note You should not use it if OutputIterator_t is back_inserter.
     */
    template <class InputIterator1_t, class InputIterator2_t, class OutputIterator_t, class BinaryFunction,
    class = std::enable_if_t<
                          std::is_assignable_v<
                              decltype(*std::declval<OutputIterator_t>()), 
                              decltype(std::declval<BinaryFunction>()(
                                  std::declval<typename IteratorValueType<InputIterator1_t>::value_type>(),
                                  std::declval<typename IteratorValueType<InputIterator2_t>::value_type>()
                                      ))
                                              >
                                      >
              >
    void transform_non_back_inserter(InputIterator1_t begin1, InputIterator1_t end1, InputIterator2_t begin2, OutputIterator_t output, BinaryFunction &&binaryFunction);
  
    template < std::forward_iterator InputIterator_t, class HashFunction = std::hash<::_helpers::IteratorValueType_t<InputIterator_t>>, class Comparator = std::less<::_helpers::IteratorValueType_t<InputIterator_t>>>
    void bitonic_sort(InputIterator_t begin, InputIterator_t end, HashFunction hashFunction = HashFunction(), Comparator comparator = Comparator());

    template < std::forward_iterator InputIterator_t, class HashFunction = std::hash<::_helpers::IteratorValueType_t<InputIterator_t>>, class Comparator = std::less<::_helpers::IteratorValueType_t<InputIterator_t>>>
    void odd_even_sort(InputIterator_t begin, InputIterator_t end, HashFunction hashFunction = HashFunction(), Comparator comparator = Comparator());

  private:
    template < class Hash_t, class Value_t, class Comparator >
    void bitonic_merge(std::vector<Hash_t> &hashValues, std::unordered_map<Hash_t, Value_t *> &hashTable, std::vector<Hash_t>::size_type low, std::vector<Hash_t>::size_type cnt, Comparator comparator);
  };

  template <_helpers::AddableIterator Iterator_t>
  inline _helpers::IteratorValueType<Iterator_t>::value_type OpenMPI::reduce(Iterator_t begin, Iterator_t end)
  {
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    std::vector<std::pair<Iterator_t, Iterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<std::optional<value_type>> results(ranges.size(), std::nullopt);
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        auto it = range.first;
        if (it != range.second)
        {
          value_type result = *it++;
          for (; it != range.second; it++)
          {
            result += *it;
          }
          results[i] = result;
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    std::optional<value_type> result;
    for (std::optional<value_type> &localResult : results)
    { 
      if (localResult.has_value())
      {
        if (result.has_value())
        {
          *result += std::move(localResult).value();
        }
        else
        {
          result = std::move(localResult).value();
        }
      }
    }
    if (not result.has_value())
      throw std::logic_error("No values passed in reduce algorithm");
    return *result;
  }

  template<_helpers::AddableIterator Iterator_t>
  inline _helpers::IteratorValueType<Iterator_t>::value_type OpenMPI::reduce(Iterator_t begin, Iterator_t end, IteratorValueType<Iterator_t>::value_type initValue)
  {
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    std::vector<std::pair<Iterator_t, Iterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<std::optional<value_type>> results(ranges.size(), std::nullopt);
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        auto it = range.first;
        if (it != range.second)
        {
          value_type result = *it++;
          for (; it != range.second; it++)
          {
            result += *it;
          }
          results[i] = result;
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    auto result = initValue;
    for (std::optional<value_type> &localResult : results)
    { 
      if (localResult.has_value())
        result += std::move(localResult).value();
    }
    return result;
  }

  template<_helpers::AddableIterator Iterator_t, _helpers::Predicate<typename _helpers::IteratorValueType<Iterator_t>::value_type> Predicate>
  inline Iterator_t OpenMPI::find_if(Iterator_t begin, Iterator_t end, Predicate && unaryFunction)
  {
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    std::vector<std::pair<Iterator_t, Iterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<std::optional<Iterator_t>> results(ranges.size(), std::nullopt);
    bool found = false;
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        for (auto it = range.first; it != range.second; it++)
        {
          bool local_found = false;
          #pragma omp atomic read
            local_found = found;
          if (local_found)
            break;
          if (unaryFunction(*it))
          {
            results[i] = it;
            #pragma omp atomic write
            found = true;
          }
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    for (std::optional<Iterator_t> &localResult : results)
    { 
      if (localResult.has_value())
        return std::move(localResult).value();
    }
    return end;
  }

  template <class Iterator_t, _helpers::Predicate<typename _helpers::IteratorValueType<Iterator_t>::value_type> Predicate>
  inline long long OpenMPI::count_if(Iterator_t begin, Iterator_t end, Predicate &&unaryFunction)
  {
    using Count_t = long long;
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    std::vector<std::pair<Iterator_t, Iterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<Count_t> results(ranges.size(), 0);
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        for (auto it = range.first; it != range.second; it++)
        {
          if (unaryFunction(*it))
          {
            results[i]++;
          }
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    Count_t count = std::reduce(results.begin(), results.end());
    return count;
  }

  template <class InputIterator_t, class OutputIterator_t, class UnaryFunction, class>
  inline void OpenMPI::transform(InputIterator_t begin, InputIterator_t end, OutputIterator_t output, UnaryFunction &&unaryFunction)
  {
    using InputValue_t = ::_helpers::IteratorValueType<InputIterator_t>::value_type;
    using UnaryFunctionReturn_t = std::invoke_result_t<UnaryFunction, InputValue_t>;
    std::vector<std::pair<InputIterator_t, InputIterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<std::vector<UnaryFunctionReturn_t>> results(ranges.size());
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        for (auto it = range.first; it != range.second; it++)
        {
          results[i].push_back(unaryFunction(*it));
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    for (auto &localResult : results)
    {
      for (auto &value : localResult)
      {
        if constexpr (std::is_move_assignable_v<UnaryFunctionReturn_t>)
        {
          *output++ = std::move(value);
        }
        else
        {
          *output++ = value;
        }
      }
    }
  }

  template <class InputIterator_t, class OutputIterator_t, class UnaryFunction, class>
  inline void OpenMPI::transform_non_back_inserter(InputIterator_t begin, InputIterator_t end, OutputIterator_t output, UnaryFunction &&unaryFunction)
  {
    std::vector<std::pair<InputIterator_t, InputIterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        auto localOutput = output;
        std::advance(localOutput, range.first - begin);
        for (auto it = range.first; it != range.second; it++)
        {
          *localOutput++ = unaryFunction(*it);
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
  }

  template <class InputIterator1_t, class InputIterator2_t, class OutputIterator_t, class BinaryFunction, class>
  inline void OpenMPI::transform(InputIterator1_t begin1, InputIterator1_t end1, InputIterator2_t begin2, OutputIterator_t output, BinaryFunction &&binaryFunction)
  {
    using InputValue1_t = ::_helpers::IteratorValueType<InputIterator1_t>::value_type;
    using InputValue2_t = ::_helpers::IteratorValueType<InputIterator2_t>::value_type;
    using BinaryFunctionReturn_t = std::invoke_result_t<BinaryFunction, InputValue1_t, InputValue2_t>;
    std::vector<std::pair<InputIterator1_t, InputIterator1_t>> ranges = generateRanges(begin1, end1, omp_get_max_threads());
    std::vector<std::vector<BinaryFunctionReturn_t>> results(ranges.size());
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        auto localBegin2 = begin2;
        std::advance(localBegin2, std::distance(range.first, begin1));
        for (auto it = range.first; it != range.second; it++, localBegin2++)
        {
          results[i].push_back(binaryFunction(*it, *localBegin2));
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
    for (auto &localResult : results)
    {
      for (auto &value : localResult)
      {
        if constexpr (std::is_move_assignable_v<BinaryFunctionReturn_t>)
        {
          *output++ = std::move(value);
        }
        else
        {
          *output++ = value;
        }
      }
    }
  }

  template <class InputIterator1_t, class InputIterator2_t, class OutputIterator_t, class BinaryFunction, class>
  inline void OpenMPI::transform_non_back_inserter(InputIterator1_t begin1, InputIterator1_t end1, InputIterator2_t begin2, OutputIterator_t output, BinaryFunction &&binaryFunction)
  {
    std::vector<std::pair<InputIterator1_t, InputIterator1_t>> ranges = generateRanges(begin1, end1, omp_get_max_threads());
    try
    {
      #pragma omp parallel for
      for (auto i = 0; i < ranges.size(); ++i)
      {
        const auto &range = ranges[i];
        auto localOutput = output;
        auto localBegin2 = begin2;
        std::advance(localBegin2, range.first - begin1);
        std::advance(localOutput, range.first - begin1);
        for (auto it = range.first; it != range.second; it++, localBegin2++)
        {
          *localOutput++ = binaryFunction(*it, *localBegin2);
        }
      }
    }
    catch(const std::exception &e)
    {
      throw;
    }
  }

  template<std::forward_iterator InputIterator_t, class HashFunction, class Comparator>
  inline void OpenMPI::bitonic_sort(InputIterator_t begin, InputIterator_t end, HashFunction hashFunction, Comparator comparator)
  {
    using value_type = ::_helpers::IteratorValueType_t<InputIterator_t>;
    using hash_t = std::invoke_result_t<HashFunction, value_type>;

    auto length = std::distance(begin, end);
    assert(("Array length must be a power of 2", ((length - 1) & length) == 0));

    auto [hashValues, hashTable] = hash_sequence< InputIterator_t, hash_t, value_type >(begin, end, hashFunction, comparator);

    // Sort first half in ascending order
    std::sort(std::execution::par_unseq, hashValues.begin(), hashValues.begin() + length, 
    [&hashTable, &comparator](hash_t lhs, hash_t rhs)
    {
      return comparator(*hashTable[lhs], *hashTable[rhs]);
    });

    // Sort second half in descending order
    std::sort(std::execution::par_unseq, hashValues.begin() + length, hashValues.end(), 
    [&hashTable, &comparator](hash_t lhs, hash_t rhs)
    {
      return not comparator(*hashTable[lhs], *hashTable[rhs]);
    });

    bitonic_merge(hashValues, hashTable, 0, hashValues.size(), comparator);

    placeElementsInCorrectPositions(begin, end, hashFunction, hashValues, hashTable);
  }

  template <std::forward_iterator InputIterator_t, class HashFunction, class Comparator>
  inline void OpenMPI::odd_even_sort(InputIterator_t begin, InputIterator_t end,
                                     HashFunction hashFunction,
                                     Comparator comparator)
  {
    using value_type = ::_helpers::IteratorValueType_t<InputIterator_t>;
    using hash_t = std::invoke_result_t<HashFunction, value_type>;

    if (begin == end)
      return;

    auto [hashValues, hashTable] = hash_sequence< InputIterator_t, hash_t, value_type >(begin, end, hashFunction, comparator);

    using size_type = decltype(hashValues.size());
    size_type swapCount = 0;
    auto inplaceComparator = createInplaceComparator(comparator, hashTable);
    do
    {
      swapCount = 0;

      #pragma parallel for
      for (size_type i = 1; i < hashValues.size(); i += 2)
      {
        swapCount += static_cast<size_type>(inplaceComparator(hashValues[i-1], hashValues[i]));
      }

      #pragma parallel for
      for (size_type i = 2; i < hashValues.size(); i += 2)
      {
        swapCount += static_cast<size_type>(inplaceComparator(hashValues[i-1], hashValues[i]));
      }
    } while (swapCount > 0);
    
    placeElementsInCorrectPositions(begin, end, hashFunction, hashValues, hashTable);
  }

  template<class Hash_t, class Value_t, class Comparator>
  inline void OpenMPI::bitonic_merge(std::vector<Hash_t>& hashValues, 
                                     std::unordered_map<Hash_t, Value_t *>& hashTable, 
                                     std::vector<Hash_t>::size_type low, 
                                     std::vector<Hash_t>::size_type cnt,
                                     Comparator comparator)
  {
    if (cnt <= 1)
      return;
    auto k = cnt / 2;
    auto inplaceComparator = createInplaceComparator(comparator, hashTable);
    #pragma omp parallel for
    for (auto i = low; i < low + k; i++)
    {
      inplaceComparator(hashValues[i], hashValues[i + k]);
    }
    bitonic_merge(hashValues, hashTable, low, k, comparator);
    bitonic_merge(hashValues, hashTable, low + k, k, comparator);
  }

} // namespace s0m4b0dY

#endif
