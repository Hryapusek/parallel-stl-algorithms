#ifndef S0M4B0D4_PARALLEL_ALGORITHMS
#define HRYAPUSEK_PARALLEL_ALGORITHMS

#include <type_traits>
#include <vector>
#include <omp.h>

#include "s0_type_traits.hpp"

namespace s0m4b0dY
{
  class OpenMPI
  {
  public:
    template <_helpers::AddableIterator Iterator_t>
    void reduce(Iterator_t begin, Iterator_t end);
  };

  template <_helpers::AddableIterator Iterator_t>
  inline void OpenMPI::reduce(Iterator_t begin, Iterator_t end)
  {
    auto length = std::distance(begin, end);
    using value_type = _helpers::IteratorValueType<Iterator_t>::value_type;
    value_type result = 0;
    auto n_cpu = omp_get_max_threads();
    assert(length > n_cpu);
    auto jobs_on_kernel = length / n_cpu;

    std::vector<std::pair<Iterator_t, Iterator_t>> ranges;
    for (auto i = 0, it = begin; i < n_cpu; ++i)
    {
      auto left = it;
      auto right = std::advance(it, jobs_on_kernel);
      it = right;
      ranges.push_back({left, right});
    }


    std::vector<value_type> results(n_cpu);

  }
} // namespace s0m4b0dY

#endif
