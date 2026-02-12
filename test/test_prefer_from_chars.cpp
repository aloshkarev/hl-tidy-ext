// RUN: %clang_tidy -checks='-*,hl-perf-prefer-from-chars' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <string>
#include <cstdlib>

void parseNumbers(const std::string& input) {
  // CHECK: warning: std::sto* functions throw exceptions
  int val = std::stoi(input);

  // CHECK: warning: std::sto* functions throw exceptions
  double dval = std::stod(input);

  // CHECK: warning: std::to_string allocates heap memory
  std::string result = std::to_string(val);

  // CHECK: warning: C-style ato* functions have undefined behavior
  int cval = atoi(input.c_str());
}

// Good: std::from_chars (C++17).
#include <charconv>
int parseIntFast(std::string_view sv) {
  int val = 0;
  std::from_chars(sv.data(), sv.data() + sv.size(), val);
  return val;  // no warning
}
