#include "Length.h"


#if defined(DEBUG_LENGTH) && !defined(DEBUG_LENGTH_COMPRESS_OUTPUT)
#define PRINT_CACHE()  for (size_t i = first; i < last; i += step) { DEBUG_OS << std::setw(4) << cache[i]; } DEBUG_OS << std::endl << std::endl;
#else
#define PRINT_CACHE()
#endif

/**
 * @brief
 * Partition by cache[first] for items at
 * index = (first + step * i) for i in [0, (last - first) / step).
 * After the function, for any i and j such that cache[i] <= cache[first] < cache[j], it holds that i < j.
 * cache[first] would not be moved.
 * Returns the number of items that are less than or equal to the first item.
 *
 * @param cache The vector to partition.
 * @param first The starting index of the range.
 * @param last The ending index of the range (exclusive).
 * @param step The step size for partitioning.
 * @return The number of items that are less than or equal to the first item.
 */
template <typename L>
size_t partitionByFirst(std::vector<L>&cache, size_t first = 0, size_t last = 0, size_t step = 1);


template <typename L>
void linearLocateMinQ(std::vector<L>& cache, size_t q, size_t first, size_t last, size_t step) {
    if (last == 0) { last = cache.size(); }
    size_t n = (last - first + step - 1) / step; // number of items in the range
    assert(first < last && step > 0 && q > 0 && q <= n && "Invalid parameters for linearLocateMinQ");
    DEBUG_LENGTH_LOG("linearLocateMinQ called with q=" << q << ", first=" << first << ", last=" << last << ", step=" << step << ", n=" << n << "; Contents:");
	PRINT_CACHE();
    // if n == 1, do nothing.
    if (n == 2) {
        // if n == 2, short circuit.
        if ((cache[first] < cache[first + step]) ^ (q == 1)) {std::swap(cache[first], cache[first + step]); }
    } else if (n >= 3) {
		// if n >= 3, we first try to find a good estimate of the median.
        // if 3 <= n <= 5, any common sorting algorithm would work.
        // Another solution is we blindly use the first element as the median estimate.
		// If n > 5, we use the median of medians algorithm.
        if (n > 5) {
            // If there are more items, we partition the array into groups of 5.
            // Find the median of each group and swap to the first position.
            // This would be a good estimate of the median of the whole array, and will be swapped to cache[first].
            size_t n_group = (n + 4) / 5; // number of groups
            size_t five_step = step * 5; // new step length
            for (size_t i = first; i < last; i += five_step) {
                if (i + five_step < last) {
                    // This is not the last group.
                    linearLocateMinQ<L>(cache, 3, i, i + five_step, step);
                }
                else {
                    // This is the last group.
                    size_t last_group_size = (last - i + step - 1) / step;
                    linearLocateMinQ<L>(cache, (last_group_size + 1) / 2, i, last, step);
                }
            }
            // Now the medians of the groups are at positions first, first + five_step, first + 2 * five_step, ...
            // We recursively find the median of these medians.
            linearLocateMinQ<L>(cache, (n_group + 1) / 2, first, last, five_step);
        }

        // number of items which are <= median estimate
        size_t n_no_greater_estimate = partitionByFirst<L>(cache, first, last, step);

        if (q < n_no_greater_estimate) {
            linearLocateMinQ<L>(cache, q, first + step, first + step * n_no_greater_estimate, step);
            std::swap(cache[first], cache[first + step]);
        } else if (q > n_no_greater_estimate) {
            size_t new_first = first + step * n_no_greater_estimate;
            linearLocateMinQ<L>(cache, q - n_no_greater_estimate, new_first, last, step);
			// The q-th smallest element has been swapped to the position new_first.
			// Now we swap it back to the position first and return.
            std::swap(cache[first], cache[new_first]);
        }
		// If q == n_no_greater_estimate, then the q-th smallest element has been swapped to the position first, nothing to do.
    }
    DEBUG_LENGTH_LOG("Leaving linearLocateMinQ with q=" << q << ", first=" << first << ", last=" << last << ", step=" << step << ", n=" << n << "; Contents:");
    PRINT_CACHE();
}


template <typename L>
size_t partitionByFirst(std::vector<L>& cache, size_t first, size_t last, size_t step) {
    if (last == 0) { last = cache.size(); }
    size_t n = (last - first + step - 1) / step; // number of items in the range
    assert(first < last && step > 0 && "Invalid parameters for partitionByFirst");
    DEBUG_LENGTH_LOG("partitionByFirst called with first=" << first << ", last=" << last << ", step=" << step << ", n=" << n << "; Contents:");
    PRINT_CACHE();

    size_t res;
    if (n == 1) { res = 1; }
    else if (n == 2) { res = (cache[first] < cache[first + step]) ? 1 : 2; }
    else {
        // Partition the array by cache[first].
        auto& firstValue = cache[first];
        auto start = first;
        auto end = first + step * (n - 1);
        while (start <= end) {
            while (start < last && cache[start] <= firstValue && start <= end) { start += step; }
            while (end >= first && cache[end] > firstValue && start <= end) { end -= step; }
            if (start < end && start < last && end >= first) {
                std::swap(cache[start], cache[end]);
                start += step;
                end -= step;
            }
        }
        res = (end - first) / step + 1;
    }
    DEBUG_LENGTH_LOG("Leaving partitionByFirst with first=" << first << ", last=" << last << ", step=" << step << ", n=" << n << "; Contents:");
    PRINT_CACHE();
    return res;
}


#ifdef DEBUG_LENGTH
template void linearLocateMinQ<int>(std::vector<int>& cache, size_t q, size_t first, size_t last, size_t step);
template size_t partitionByFirst<int>(std::vector<int>& cache, size_t first, size_t last, size_t step);
#endif

template void linearLocateMinQ<Length>(std::vector<Length>& cache, size_t q, size_t first, size_t last, size_t step);
template size_t partitionByFirst<Length>(std::vector<Length>& cache, size_t first, size_t last, size_t step);
