// RUN: %clang_tidy -checks='-*,hl-perf-avoid-std-regex' %s -- -std=c++17 \
// RUN:   2>&1 | %FileCheck %s

#include <regex>
#include <string>

void parseInput(const std::string& input) {
  // CHECK: warning: std::regex has extremely poor performance
  std::regex pattern(R"(\d{4}-\d{2}-\d{2})");

  // CHECK: warning: std::regex has extremely poor performance
  std::smatch match;
  std::regex_search(input, match, pattern);
}

// Good: use string::find for simple patterns.
bool hasDate(const std::string& input) {
  return input.find('-') != std::string::npos;  // no warning
}
