// RUN: %clang_tidy -checks='-*,hl-perf-prefer-string-view' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <string>
#include <string_view>

// CHECK: warning: 'const std::string&' parameter causes potential heap allocation
void processName(const std::string& name) {
  // Reading only — string_view would be better.
}

// Good: string_view for read-only access.
void processNameFast(std::string_view name) {
  // no warning
}

// Good: by-value when storing (move semantics).
void storeName(std::string name) {
  // no warning
}
