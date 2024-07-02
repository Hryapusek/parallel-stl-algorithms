#ifndef S0M4B0D4_PARALLEL_ALGORITHMS
#define S0M4B0D4_PARALLEL_ALGORITHMS

#include <type_traits>
#include <vector>
#include <omp.h>

#include "CommonUtils/s0_type_traits.hpp"
#include "CommonUtils/s0_utils.hpp"

namespace s0m4b0dY
{
  class OpenMPI
  {
  public:
    template <_helpers::AddableIterator Iterator_t>
    _helpers::IteratorValueType<Iterator_t>::value_type reduce(Iterator_t begin, Iterator_t end);
  };

  template <_helpers::AddableIterator Iterator_t>
  inline _helpers::IteratorValueType<Iterator_t>::value_type OpenMPI::reduce(Iterator_t begin, Iterator_t end)
  {
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    const value_type init_value = 0;
    std::vector<std::pair<Iterator_t, Iterator_t>> ranges = generateRanges(begin, end, omp_get_max_threads());
    std::vector<value_type> results(ranges.size(), init_value);
    #pragma omp parallel for
    for (auto i = 0; i < ranges.size(); ++i)
    {
      const auto &range = ranges[i];
      value_type result = init_value;
      for (auto it = range.first; it != range.second; it++)
      {
        result += *it;
      }
      results[i] = result;
    }
    auto result = init_value;
    for (const auto &local_result : results)
    {
      result += local_result;
    }
    return result;
  }
} // namespace s0m4b0dY

#endif
