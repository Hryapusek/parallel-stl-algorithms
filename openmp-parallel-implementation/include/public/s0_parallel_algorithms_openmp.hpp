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

} // namespace s0m4b0dY

#endif
