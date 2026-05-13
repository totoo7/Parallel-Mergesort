#include "algorithms/merge_sort.hpp"

void parallel_merge_sort(std::vector<int>& arr, ThreadPool& pool) {
    int n = arr.size();

    if (n <= 1) return;

    int thread_count = pool.size();
    int chunk_size = n / thread_count;

    std::vector<std::future<void>> futures;

    for (int i = 0; i < thread_count; i++) {

        int left = i * chunk_size;
        int right = (i == thread_count - 1)
                        ? n - 1
                        : (left + chunk_size - 1);

        futures.push_back(pool.enqueue([&arr, left, right]() {
            std::sort(arr.begin() + left, arr.begin() + right + 1);
        }));
    }

    for (auto& f : futures)
        f.get();

    std::vector<int> temp;
    temp.reserve(n);

    int step = chunk_size;

    while (step < n) {

        for (int i = 0; i + step < n; i += 2 * step) {

            int left = i;
            int mid = i + step - 1;
            int right = std::min(i + 2 * step - 1, n - 1);

            std::vector<int> merged;
            merged.reserve(right - left + 1);

            int a = left, b = mid + 1;

            while (a <= mid && b <= right) {
                if (arr[a] <= arr[b]) merged.push_back(arr[a++]);
                else merged.push_back(arr[b++]);
            }

            while (a <= mid) merged.push_back(arr[a++]);
            while (b <= right) merged.push_back(arr[b++]);

            for (int k = 0; k < merged.size(); k++) {
                arr[left + k] = merged[k];
            }
        }

        step *= 2;
    }
}