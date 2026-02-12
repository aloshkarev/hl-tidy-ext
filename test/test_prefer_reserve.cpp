// RUN: %clang_tidy -checks='-*,hl-perf-prefer-reserve' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <vector>

void buildVector(int n) {
  std::vector<int> result;
  // CHECK: warning: push_back/emplace_back inside a loop without reserve()
  for (int i = 0; i < n; ++i) {
    result.push_back(i);
  }
}

// Good: reserve before the loop.
void buildVectorFast(int n) {
  std::vector<int> result;
  result.reserve(n);
  for (int i = 0; i < n; ++i) {
    result.push_back(i);  // still flagged (heuristic), but reserve is present
  }
}
