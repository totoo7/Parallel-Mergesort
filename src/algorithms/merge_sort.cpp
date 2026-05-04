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

static void merge_sort_helper(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;
    merge_sort_helper(arr, left, mid);
    merge_sort_helper(arr, mid + 1, right);

    merge(arr, left, mid, right);
}

void merge_sort(std::vector<int>& arr) {
    if (arr.empty()) return;
    merge_sort_helper(arr, 0, arr.size() - 1);
}