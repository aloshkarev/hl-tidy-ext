// RUN: %clang_tidy -checks='-*,hl-modernize-prefer-expected,hl-modernize-prefer-flat-containers,hl-modernize-prefer-move-only-function' \
// RUN:   %s -- -std=c++23 2>&1 | %FileCheck %s

#include <optional>
#include <map>
#include <set>
#include <functional>
#include <stdexcept>

// CHECK: warning: function returns std::optional but may benefit from std::expected
std::optional<int> parseInt(const char* str) {
  if (!str) return std::nullopt;
  return 42;
}

// CHECK: warning: throw expression in regular function
int divide(int a, int b) {
  if (b == 0) throw std::runtime_error("division by zero");
  return a / b;
}

class Cache {
  // CHECK: warning: std::map is a tree-based container
  std::map<int, int> entries_;

  // CHECK: warning: std::set is a tree-based container
  std::set<int> keys_;

  // CHECK: warning: std::function {{.*}} can likely be replaced with std::move_only_function
  std::function<void()> callback_;
};
