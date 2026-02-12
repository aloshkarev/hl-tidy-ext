// Test file for new C++23 checks.
// Run:
//   clang-tidy-20 -load ./build/HlTidyModule.so \
//     -checks='-*,hl-modernize-prefer-unreachable,hl-modernize-prefer-to-underlying,hl-modernize-prefer-print' \
//     test/test_cpp23_new_checks.cpp -- -std=c++23

#include <cstdio>
#include <cstdlib>
#include <iostream>

// --- hl-modernize-prefer-unreachable ---

enum class Direction { North, South, East, West };

int dirToInt(Direction d) {
  switch (d) {
    case Direction::North: return 0;
    case Direction::South: return 1;
    case Direction::East:  return 2;
    case Direction::West:  return 3;
  }
  // Should trigger: __builtin_unreachable is GCC/Clang-specific.
  __builtin_unreachable();
}

// --- hl-modernize-prefer-to-underlying ---

enum class HttpStatus : int {
  Ok = 200,
  NotFound = 404,
  InternalError = 500
};

int statusCode(HttpStatus s) {
  // Should trigger: static_cast of scoped enum.
  return static_cast<int>(s);
}

// --- hl-modernize-prefer-print ---

void output() {
  // Should trigger: printf is not type-safe.
  printf("Request %d processed in %f ms\n", 42, 1.5);

  // Should trigger: cout << chain.
  std::cout << "Hello" << " world" << std::endl;
}
