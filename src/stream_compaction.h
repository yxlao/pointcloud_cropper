#pragma once

#include <functional>

template <typename T, typename index_t>
void GetValidIndices(const T* src,
                     index_t* valid_indices,
                     index_t N,
                     std::function<bool(const T&)> is_valid) {
    index_t total_sum = tbb::parallel_scan(
            tbb::blocked_range<index_t>(0, N), 0,
            [&](const tbb::blocked_range<index_t>& r, index_t sum,
                bool is_final_scan) -> index_t {
                index_t temp = sum;
                for (index_t i = r.begin(); i < r.end(); ++i) {
                    if (is_valid(src[i])) {
                        if (is_final_scan) {
                            valid_indices[temp] = i;
                        }
                        temp += 1;
                    }
                }
                return temp;
            },
            [](int left, int right) { return left + right; });
}
