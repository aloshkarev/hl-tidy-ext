// Test file for the 6 new checks added in the second iteration.
// Run:
//   clang-tidy-20 -load ./build/HlTidyModule.so \
//     -checks='-*,hl-perf-avoid-dynamic-cast,hl-perf-avoid-std-bind,hl-perf-prefer-emplace,hl-perf-prefer-noexcept-move,hl-perf-avoid-cout-cerr,hl-perf-avoid-virtual-in-loop' \
//     test/test_new_checks.cpp -- -std=c++17

#include <functional>
#include <iostream>
#include <string>
#include <vector>

// --- hl-perf-avoid-dynamic-cast ---

struct Base {
  virtual ~Base() = default;
  virtual void process() {}
};
struct Derived : Base {
  void process() override {}
  int extra = 42;
};

void useDynamicCast(Base* b) {
  // Should trigger: dynamic_cast requires RTTI
  if (auto* d = dynamic_cast<Derived*>(b)) {
    d->extra = 0;
  }
}

// --- hl-perf-avoid-std-bind ---

int add(int a, int b) { return a + b; }

void useStdBind() {
  // Should trigger: std::bind creates type-erased wrapper
  auto bound = std::bind(add, 1, std::placeholders::_1);
  bound(2);
}

// Good: lambda (zero overhead).
void useLambda() {
  auto bound = [](int b) { return add(1, b); };
  bound(2);
}

// --- hl-perf-prefer-emplace ---

struct Heavy {
  Heavy(int a, double b) : x(a), y(b) {}
  int x;
  double y;
};

void useEmplace() {
  std::vector<Heavy> vec;
  // Should trigger: push_back with temporary
  vec.push_back(Heavy(1, 2.0));

  // Good: emplace_back constructs in-place.
  vec.emplace_back(3, 4.0);
}

// --- hl-perf-prefer-noexcept-move ---

class Widget {
  std::string data_;
public:
  // Should trigger: move ctor without noexcept
  Widget(Widget&& other) : data_(std::move(other.data_)) {}

  // Should trigger: move assignment without noexcept
  Widget& operator=(Widget&& other) {
    data_ = std::move(other.data_);
    return *this;
  }

  Widget() = default;
  Widget(const Widget&) = default;
  Widget& operator=(const Widget&) = default;
};

// Good: noexcept move.
class FastWidget {
  std::string data_;
public:
  FastWidget(FastWidget&& other) noexcept : data_(std::move(other.data_)) {}
  FastWidget& operator=(FastWidget&& other) noexcept {
    data_ = std::move(other.data_);
    return *this;
  }
  FastWidget() = default;
};

// --- hl-perf-avoid-cout-cerr ---

void logMessage() {
  // Should trigger: std::cout uses global mutex
  std::cout << "request processed" << '\n';

  // Should trigger: std::cerr uses global mutex
  std::cerr << "error occurred" << '\n';
}

// --- hl-perf-avoid-virtual-in-loop ---

void processItems(std::vector<Base*>& items) {
  // Should trigger: virtual call inside loop
  for (auto* item : items) {
    item->process();
  }
}
