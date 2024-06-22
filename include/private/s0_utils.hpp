#ifndef S0M4B0DY_UTILS_HPP
#define S0M4B0DY_UTILS_HPP

#include <vector>


template< class Iterator_t >
auto generateRanges(Iterator_t begin, Iterator_t end);


template< class Iterator_t >
auto generateRanges(Iterator_t begin, Iterator_t end, int max_number_of_threads)
{
    auto n_cpu = max_number_of_threads;
    auto jobs_on_kernel = length / n_cpu;
    auto length = std::distance(begin, end);

    std::vector<std::pair<Iterator_t, Iterator_t>> ranges;

    for (auto i = 0, it = begin; i < n_cpu; ++i)
    {
      auto left = it;
      auto right = std::advance(it, jobs_on_kernel);
      it = right;
      ranges.push_back({left, right});
    }
    return ranges;
}

#endif
