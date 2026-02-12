// RUN: %clang_tidy -checks='-*,hl-perf-avoid-std-function' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <functional>

// CHECK: warning: std::function causes heap allocation and type-erasure overhead
std::function<void()> global_callback;

class Server {
  // CHECK: warning: std::function causes heap allocation and type-erasure overhead
  std::function<void(int)> on_request_;

public:
  // CHECK: warning: std::function causes heap allocation and type-erasure overhead
  void setHandler(std::function<void(int)> handler) {
    on_request_ = std::move(handler);
  }
};

void processWithCallback(std::function<int(int)> fn) {
  // CHECK: warning: std::function causes heap allocation and type-erasure overhead
  fn(42);
}

// Good: template parameter — zero overhead.
template <typename Fn>
void processWithTemplate(Fn&& fn) {
  fn(42);  // no warning
}

// Good: function pointer — zero overhead.
void processWithFnPtr(int (*fn)(int)) {
  fn(42);  // no warning
}
