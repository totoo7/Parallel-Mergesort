#ifndef MERGE_SORT
#define MERGE_SORT

#include <server/thread_pool.hpp>
#include <vector>

void parallel_merge_sort(std::vector<int>& arr, ThreadPool& thread_pool);

#endif