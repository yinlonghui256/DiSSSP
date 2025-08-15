#include "Length.h"


Length linearLocateMinQ(std::vector<Length>& cache, size_t q, size_t first, size_t last, size_t step) {
    if (last == 0) { last = cache.size(); }
    size_t n = (last - first + step - 1) / step; // number of items in the range

    if (first >= last || step == 0 || q == 0 || q > n) {
        throw std::out_of_range("Invalid parameters for linearLocateMinK");
    }

    if (n <= 5) {
        // If there are 5 or fewer items (O(1)).
        // Bubble sort is still O(1) time.
        for (size_t i = first; i < last; i += step) {
            for (size_t j = i + step; j < last; j += step) {
                if (cache[i] > cache[j]) {
                    std::swap(cache[i], cache[j]);
                }
            }
        }
        std::swap(cache[first], cache[first + (q - 1) * step]);
        return cache[first];
    } else {
        // If there are more items, we partition the array into groups of 5.
        // Find the median of each group and swap to the first position.
        size_t n_group = (n + 4) / 5; // number of groups
        size_t five_step = step * 5; // new step length
        for (size_t i = first; i < last; i += five_step) {
            if (i + five_step < last) {
                // This is not the last group.
                linearLocateMinQ(cache, 3, i, i + five_step, step);
            } else {
                // This is the last group.
                size_t last_group_size = (last - i + step - 1) / step;
                linearLocateMinQ(cache, (last_group_size + 1) / 2, i, last, step);
            }
        }
        // Now the medians are at positions first, first + five_step, first + 2 * five_step, ...
        // We recursively find the median of these medians.
        Length medianEstimate = linearLocateMinQ(cache, (n_group + 1) / 2, first, last, five_step);

        // Partition the array by the median estimate.
        auto start = cache.begin() + first;
        auto end = cache.begin() + first + step * (n - 1);
        while (start <= end) {
            while (*start <= medianEstimate && start <= end) { start += step; }
            while (*end > medianEstimate && start <= end) { end -= step; }
            if (start < end) {
                std::swap(*start, *end);
                start += step;
                end -= step;
            }
        }

        // number of items <= medianEstimate
        size_t n_no_greater_estiamte = (end - (cache.begin() + first)) / step + 1;

        if (q < n_no_greater_estiamte) {
            return linearLocateMinQ(cache, q, first, first + step * n_no_greater_estiamte, step);
        } else if (q == n_no_greater_estiamte) {
            // The q-th smallest element is the median estimate,
            // And it has been swapped to the position first.
            return cache[first];
        } else {
            size_t new_first = first + step * n_no_greater_estiamte;
            linearLocateMinQ(cache, q - n_no_greater_estiamte, new_first, last, step);
            std::swap(cache[first], cache[new_first]);
            return cache[first];
        }
    }
}