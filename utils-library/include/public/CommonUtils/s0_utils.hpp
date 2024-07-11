#ifndef S0M4B0DY_UTILS_HPP
#define S0M4B0DY_UTILS_HPP

#include <vector>

template <class Iterator_t>
std::vector<std::pair<Iterator_t, Iterator_t> > generateRanges(Iterator_t begin, Iterator_t end, int max_number_of_threads);

template <class Iterator_t>
std::vector<std::pair<Iterator_t, Iterator_t> > generateRanges(Iterator_t begin, Iterator_t end, int max_number_of_threads)
{
	auto length = std::distance(begin, end);
	auto n_cpu = max_number_of_threads;
	auto jobs_on_kernel = length / n_cpu;
	auto minimum_required_jobs_for_parallel_execution = 20;
	if (jobs_on_kernel < minimum_required_jobs_for_parallel_execution)
	{
		return {{begin, end}};
	}

	std::vector<std::pair<Iterator_t, Iterator_t> > ranges;

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

/// @return Comparator that returns true if swap was performed.
/// @return False otherwise
template < class Comparator, class HashTable >
auto createInplaceComparator(Comparator &comparator, HashTable &hashTable)
{
	return [&comparator, &hashTable](auto &lhs, auto &rhs) -> bool
		   {
            auto leftIt = hashTable.find(lhs)->second;
            auto rightIt = hashTable.find(rhs)->second;
			   if (comparator(*leftIt, *rightIt))
			   {
				   return false;
			   }
			   else
			   {
                if (comparator(*rightIt, *leftIt))
                {
				   std::swap(lhs, rhs);
				   return true;
                }
                else
                {
                    return false;
                }
			   }
		   };
}

template<std::forward_iterator InputIterator_t, class Hash_t, class Value_t, class HashFunction, class Comparator>
inline std::pair<std::vector<Hash_t>, std::unordered_multimap<Hash_t, Value_t *> > hash_sequence(InputIterator_t begin, InputIterator_t end, HashFunction hashFunction, Comparator comparator)
{
	std::unordered_multimap<Hash_t, Value_t *> hashTable;
	std::vector<Hash_t> hashedValues;

	for (auto it = begin; it != end; it++)
	{
		auto hash = hashFunction(*it);
		hashTable.insert({hash, &*it});
		hashedValues.push_back(hash);
        if (hash > 100000000ull)
        {
            std::cout << "sus";
        }
	}

	return {std::move(hashedValues), std::move(hashTable)};
}

template< std::forward_iterator InputIterator_t, class HashFunction, class Hash_t, class Value_t >
void placeElementsInCorrectPositions(InputIterator_t begin, InputIterator_t end, HashFunction hashFunction, const std::vector<Hash_t> &hashValues, std::unordered_multimap<Hash_t, Value_t *> &hashTable)
{
    std::unordered_multimap<Hash_t, Value_t> popValues;

    for (auto src_it = begin, correctHash = hashValues.begin(); src_it != end; ++src_it, ++correctHash)
    {
        auto hash = hashFunction(*src_it);

        // Skip elements already in the correct position
        if (hash == *correctHash)
        {
            continue;
        }

        // Move the element to popValues and erase it from hashTable
        auto range = hashTable.equal_range(hash);
        auto it = range.first;
        for (; it != range.second; ++it)
        {
            if (it->second == &(*src_it))
            {
                break;
            }
        }
        if (it != range.second)
        {
            popValues.insert({hash, std::move(*src_it)});
            hashTable.erase(it);
        }

        // Try to find the correct element in popValues
        auto foundIt = popValues.find(*correctHash);
        if (foundIt != popValues.end())
        {
            *src_it = std::move(foundIt->second);
            popValues.erase(foundIt);
        }
        else
        {
            // If not found in popValues, get it from hashTable
            auto correctRange = hashTable.equal_range(*correctHash);
            if (correctRange.first != correctRange.second)
            {
                *src_it = std::move(*(correctRange.first->second));
                hashTable.erase(correctRange.first);
            }
            else
            {
                // If we cannot find the correct element in hashTable, we have a problem
                std::cerr << "Error: could not find correct element in hashTable for hash " << *correctHash << std::endl;
                std::terminate();
            }
        }
    }
}

#endif
