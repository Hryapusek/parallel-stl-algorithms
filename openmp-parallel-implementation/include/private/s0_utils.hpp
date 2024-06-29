#ifndef S0M4B0DY_UTILS_HPP
#define S0M4B0DY_UTILS_HPP

#include <vector>

template <class Iterator_t>
std::vector<std::pair<Iterator_t, Iterator_t>> generateRanges(Iterator_t begin, Iterator_t end, int max_number_of_threads);

template <class Iterator_t>
std::vector<std::pair<Iterator_t, Iterator_t>> generateRanges(Iterator_t begin, Iterator_t end, int max_number_of_threads)
{
  auto length = std::distance(begin, end);
  auto n_cpu = max_number_of_threads;
  auto jobs_on_kernel = length / n_cpu;
  auto minimum_required_jobs_for_parallel_execution = 20;
  if (jobs_on_kernel < minimum_required_jobs_for_parallel_execution)
  {
    return {{begin, end}};
  }

  std::vector<std::pair<Iterator_t, Iterator_t>> ranges;

  for (auto i = 0, it = begin; i < n_cpu; ++i)
  {
    auto left = it;
    auto right = it;
    if (i == n_cpu - 1)
    {
      right = end;
    }
    else
    {
      std::advance(it, jobs_on_kernel);
      right = it;
    }
    ranges.push_back({left, right});
  }
  return ranges;
}

#endif
