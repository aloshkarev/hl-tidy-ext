// RUN: %clang_tidy -checks='-*,hl-modernize-prefer-jthread,hl-modernize-prefer-format,hl-modernize-prefer-span' \
// RUN:   %s -- -std=c++20 2>&1 | %FileCheck %s

#include <thread>
#include <vector>
#include <sstream>
#include <cstdio>

void startWorker() {
  // CHECK: warning: std::thread requires manual join/detach; prefer std::jthread
  std::thread worker([]() {});
  worker.join();
}

void formatString() {
  // CHECK: warning: std::stringstream has significant overhead
  std::ostringstream oss;
  oss << "value=" << 42;

  // CHECK: warning: sprintf/snprintf are not type-safe
  char buf[64];
  snprintf(buf, sizeof(buf), "value=%d", 42);
}

// CHECK: warning: 'const std::vector<T>&' parameter ties the interface
void processData(const std::vector<int>& data) {
  for (auto& v : data) {
    (void)v;
  }
}
