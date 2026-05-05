#include "algorithms/merge_sort.hpp"

static void merge(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp;

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp.push_back(arr[i++]);
        } else {
            temp.push_back(arr[j++]);
        }
    }

    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);

    for (int k = 0; k < temp.size(); k++) {
        arr[left + k] = temp[k]; 
    }
}
static void parallel_merge_sort(std::vector<int>& arr, int left, int right, ThreadPool& thread_pool) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    auto left_future = thread_pool.enqueue([&arr, left, mid, &thread_pool]() {
        parallel_merge_sort(arr, left, mid, thread_pool);
    });
    parallel_merge_sort(arr, mid + 1, right, thread_pool);

    merge(arr, left, mid, right);
}

void parallel_merge_sort(std::vector<int>& arr, ThreadPool& pool) {
    if (arr.empty()) return;
    parallel_merge_sort(arr, 0, arr.size() - 1, pool);
}