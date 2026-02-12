// Example: typical highload service code with common anti-patterns.
// Run: clang-tidy-20 -load ./build/HlTidyModule.so -checks='-*,hl-*' test/example_service.cpp -- -std=c++17

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>
#include <cstdlib>

// Anti-pattern: std::function for callback
using RequestHandler = std::function<void(int)>;

class HttpServer {
public:
  // Anti-pattern: std::shared_ptr field
  std::shared_ptr<int> config_;

  // Anti-pattern: std::list instead of std::vector
  std::list<int> connections_;

  // Anti-pattern: std::map (tree-based)
  std::map<std::string, RequestHandler> routes_;

  // Anti-pattern: const std::string& param
  void handleRequest(const std::string& path) {
    // Anti-pattern: std::regex
    std::regex route_pattern(R"(/api/v(\d+)/.*)");

    // Anti-pattern: std::endl
    std::cout << "Handling: " << path << std::endl;
  }

  // Anti-pattern: push_back in loop without reserve
  std::vector<int> buildResponse(int n) {
    std::vector<int> result;
    for (int i = 0; i < n; ++i) {
      result.push_back(i * 2);
    }
    return result;
  }

  // Anti-pattern: std::stoi
  int parsePort(const std::string& s) {
    return std::stoi(s);
  }

  // Anti-pattern: atoi
  int parsePortC(const char* s) {
    return atoi(s);
  }

  // Anti-pattern: std::to_string
  std::string formatId(int id) {
    return std::to_string(id);
  }
};
