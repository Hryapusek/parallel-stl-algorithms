#ifndef S0M4B0D4_PARALLEL_ALGORITHMS
#define S0M4B0D4_PARALLEL_ALGORITHMS

#include <type_traits>
#include <vector>
#include <optional>
#include <atomic>
#include <omp.h>

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

} // namespace s0m4b0dY

#endif
