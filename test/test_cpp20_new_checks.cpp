// Test file for new C++20 checks.
// Run:
//   clang-tidy-20 -load ./build/HlTidyModule.so \
//     -checks='-*,hl-modernize-prefer-starts-ends-with,hl-modernize-prefer-contains,hl-modernize-prefer-erase-if' \
//     test/test_cpp20_new_checks.cpp -- -std=c++20

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// --- hl-modernize-prefer-starts-ends-with ---

bool checkPrefix(const std::string& url) {
  // Should trigger: find(x) == 0 scans entire string.
  return url.find("/api/") == 0;
}

bool checkPrefixSubstr(const std::string& s) {
  // Should trigger: substr ALLOCATES a temporary.
  return s.substr(0, 4) == "http";
}

bool checkPrefixCompare(const std::string& s) {
  // Should trigger: compare is verbose.
  return s.compare(0, 5, "https") == 0;
}

// Good: starts_with (C++20).
bool checkPrefixGood(const std::string& url) {
  return url.starts_with("/api/");  // no warning
}

// --- hl-modernize-prefer-contains ---

bool hasKey(const std::map<int, int>& m, int k) {
  // Should trigger: find(k) != end() is verbose.
  return m.find(k) != m.end();
}

bool hasKeyCount(const std::unordered_map<int, int>& m, int k) {
  // Should trigger: count(k) > 0.
  return m.count(k) > 0;
}

// Good: contains (C++20).
bool hasKeyGood(const std::map<int, int>& m, int k) {
  return m.contains(k);  // no warning
}

// --- hl-modernize-prefer-erase-if ---

void removeOdds(std::vector<int>& v) {
  // Should trigger: erase-remove idiom.
  v.erase(std::remove_if(v.begin(), v.end(),
                          [](int x) { return x % 2 != 0; }),
          v.end());
}

// Good: std::erase_if (C++20).
void removeOddsGood(std::vector<int>& v) {
  std::erase_if(v, [](int x) { return x % 2 != 0; });  // no warning
}
