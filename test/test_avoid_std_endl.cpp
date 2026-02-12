// RUN: %clang_tidy -checks='-*,hl-perf-avoid-std-endl' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <iostream>

void logMessage() {
  // CHECK: warning: std::endl forces a stream flush on every call
  std::cout << "Hello" << std::endl;

  // Good: '\n' does not flush.
  std::cout << "World" << '\n';  // no warning
}
