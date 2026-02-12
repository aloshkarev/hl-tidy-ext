// RUN: %clang_tidy -checks='-*,hl-perf-prefer-vector' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <list>
#include <forward_list>
#include <vector>

class DataStore {
  // CHECK: warning: std::list has poor cache locality
  std::list<int> items_;

  // CHECK: warning: std::forward_list has poor cache locality
  std::forward_list<double> cache_;

  // Good: contiguous memory.
  std::vector<int> fast_items_;  // no warning
};

void processData() {
  // CHECK: warning: std::list has poor cache locality
  std::list<int> temp;
  temp.push_back(1);
}
