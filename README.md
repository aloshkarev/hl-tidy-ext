# High-Load Performance Checks for clang-tidy

A clang-tidy plugin providing performance-oriented checks for high-load C++ services. Focuses on heap allocations, memory pressure, cache friendliness, thread safety, and best practices adopted by Chromium, Abseil, folly, and other large-scale projects.

## Key Features

- **Standard-aware**: checks automatically adapt to the project's C++ standard (C++17/20/23/26)
- **Replacement suggestions**: each check provides concrete alternatives based on the available standard
- **FixIt hints**: automatic code replacements where safe (e.g., `std::endl` → `'\n'`)
- **Configurable**: tunable thresholds via `.clang-tidy` options

## Checks Reference

### Core Performance (`hl-perf-*`) — all standards

| Check | Detects | Recommendation |
|---|---|---|
| `hl-perf-avoid-std-function` | `std::function` — heap allocation, type erasure overhead | Template params, `std::move_only_function` (C++23), `std::function_ref` (C++26) |
| `hl-perf-avoid-std-regex` | `std::regex` — catastrophic performance | CTRE (compile-time regex), Google RE2, hand-written parsers |
| `hl-perf-avoid-std-endl` | `std::endl` — forces stream flush | `'\n'` (**FixIt auto-replacement**) |
| `hl-perf-avoid-std-any` | `std::any` — heap allocation, RTTI | `std::variant` |
| `hl-perf-avoid-std-bind` | `std::bind` — type erasure, poor inlining | Lambdas (zero-overhead, inlineable) |
| `hl-perf-avoid-dynamic-cast` | `dynamic_cast` — RTTI overhead, vtable traversal | `static_cast` with type tags, visitor pattern |
| `hl-perf-avoid-cout-cerr` | `std::cout`/`std::cerr` — global mutex contention | Logging frameworks (spdlog, glog) |
| `hl-perf-prefer-vector` | `std::list`, `std::forward_list` — poor cache locality | `std::vector`, `std::hive` (C++26) |
| `hl-perf-prefer-string-view` | `const std::string&` parameters | `std::string_view` (C++17) |
| `hl-perf-prefer-from-chars` | `std::stoi/stol/stof`, `atoi`, `std::to_string` | `std::from_chars`/`std::to_chars` (C++17) |
| `hl-perf-prefer-unique-ptr` | `std::shared_ptr` — atomic refcount overhead | `std::unique_ptr`, raw ptr for borrowing |
| `hl-perf-prefer-reserve` | `push_back` in loop without `reserve()` | `vector::reserve()` before the loop |
| `hl-perf-prefer-emplace` | `push_back(T(...))` — unnecessary temporary | `emplace_back(...)` (in-place construction) |
| `hl-perf-prefer-noexcept-move` | Move ctor/assignment without `noexcept` | Add `noexcept` to enable vector move-optimization |
| `hl-perf-avoid-virtual-in-loop` | Virtual calls inside tight loops | CRTP, `if constexpr`, devirtualization hints |

### C++20 Modernisation (`hl-modernize-*`)

| Check | Detects | Recommendation |
|---|---|---|
| `hl-modernize-prefer-jthread` | `std::thread` | `std::jthread` — RAII, cooperative cancellation |
| `hl-modernize-prefer-format` | `std::stringstream`, `sprintf/snprintf` | `std::format`, `std::print` (C++23) |
| `hl-modernize-prefer-span` | `const std::vector<T>&` params, `T*` + size | `std::span<const T>` |
| `hl-modernize-prefer-starts-ends-with` | `s.find(x)==0`, `s.substr(0,n)==x` (allocates!), `s.compare(0,n,x)==0` | `s.starts_with(x)` / `s.ends_with(x)` — O(prefix), zero allocation |
| `hl-modernize-prefer-contains` | `m.find(k)!=m.end()`, `m.count(k)>0`, `s.find(x)!=npos` | `.contains()` — returns bool, no iterator overhead |
| `hl-modernize-prefer-erase-if` | Erase-remove idiom `v.erase(std::remove_if(...), v.end())` | `std::erase_if(container, predicate)` — uniform, single call |

### C++23 Modernisation

| Check | Detects | Recommendation |
|---|---|---|
| `hl-modernize-prefer-expected` | `std::optional` for errors, `throw` in regular functions | `std::expected<T, E>` |
| `hl-modernize-prefer-flat-containers` | `std::map/set/multimap/multiset` | `std::flat_map/flat_set` (C++23), sorted vector |
| `hl-modernize-prefer-move-only-function` | `std::function` for move-only callbacks | `std::move_only_function` |
| `hl-modernize-prefer-unreachable` | `__builtin_unreachable()`, `__assume(false)` | `std::unreachable()` — portable, same optimizer hint |
| `hl-modernize-prefer-to-underlying` | `static_cast<int>(scoped_enum)` — fragile if underlying type changes | `std::to_underlying()` — always correct type |
| `hl-modernize-prefer-print` | `printf/fprintf/puts`, `cout <<` chains | `std::print`/`std::println` — type-safe, no mutex contention, compile-time validated |

### C++26 Modernisation

| Check | Detects | Recommendation |
|---|---|---|
| `hl-modernize-prefer-function-ref` | `std::function` parameters (non-owning) | `std::function_ref` |
| `hl-modernize-prefer-inplace-vector` | Local `std::vector` with small `reserve()` | `std::inplace_vector<T, N>` |
| `hl-modernize-prefer-copyable-function` | `std::function` — no const/noexcept enforcement | `std::copyable_function` — stricter qualifiers, better SBO |
| `hl-modernize-prefer-hive` | `std::list`/`std::forward_list` — per-node alloc, cache-hostile | `std::hive` — contiguous blocks, stable ptrs, 5–20x faster iteration |

## Building

### Dependencies

- LLVM/Clang 16+ (with development packages)
- CMake 3.16+

### Installing Dependencies (Ubuntu/Debian)

```bash
# Install a specific LLVM version (e.g. 20):
sudo apt install llvm-20-dev libclang-20-dev clang-tidy-20
```

### Building the Plugin

```bash
mkdir build && cd build
cmake .. -DLLVM_VERSION=20 -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

Output: `build/HlTidyModule.so`

### Building with Tests

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DHL_TIDY_ENABLE_TESTS=ON -DLLVM_VERSION=20
cmake --build . -j$(nproc)
cmake --build . --target check-hl-tidy
```

## Usage

### Command Line

```bash
# All hl-* checks:
clang-tidy -load ./build/HlTidyModule.so \
  -checks='-*,hl-perf-*,hl-modernize-*' \
  -p build/ src/main.cpp

# Core performance checks only:
clang-tidy -load ./build/HlTidyModule.so \
  -checks='-*,hl-perf-*' \
  -p build/ src/main.cpp

# With auto-fixes (where available):
clang-tidy -load ./build/HlTidyModule.so \
  -checks='-*,hl-perf-*' \
  -fix -p build/ src/main.cpp
```

### .clang-tidy Configuration

```yaml
Checks: >
  -*,
  hl-perf-*,
  hl-modernize-*

CheckOptions:
  - key: hl-modernize-prefer-inplace-vector.MaxInplaceSize
    value: 64
```

### CMake Integration

```cmake
set(CMAKE_CXX_CLANG_TIDY
  clang-tidy
  -load=${PATH_TO_HL_TIDY}/HlTidyModule.so
  -checks=-*,hl-perf-*,hl-modernize-*
)
```

### CI/CD Integration

```bash
#!/bin/bash
set -euo pipefail

PLUGIN="./build/HlTidyModule.so"
BUILD_DIR="./build"

find src/ -name '*.cpp' -print0 | \
  xargs -0 -P$(nproc) -I{} \
    clang-tidy -load "$PLUGIN" \
      -checks='-*,hl-perf-*,hl-modernize-*' \
      -warnings-as-errors='hl-perf-avoid-std-regex,hl-perf-avoid-std-endl' \
      -p "$BUILD_DIR" {}
```

## Standard Adaptation

The plugin **automatically** detects the C++ standard from compilation flags (`-std=c++17`, `-std=c++20`, etc.). How it works:

1. Each check implements `isLanguageVersionSupported()` — if the project standard is lower than required, the check is silently deactivated
2. Inside checks, `utils::detectStandard()` adapts recommendations:
   - In a C++17 project: `std::function` → "use template parameters"
   - In a C++23 project: `std::function` → "use `std::move_only_function`"
   - In a C++26 project: `std::function` → "use `std::function_ref`"

The standard is read from `compile_commands.json` (generated by CMake with `CMAKE_EXPORT_COMPILE_COMMANDS=ON`), so if your CMake sets `CMAKE_CXX_STANDARD 17`, all checks will receive this information automatically.

## Design Principles

Based on practices from major projects:

- **Chromium**: bans `std::regex`, `std::function` in hot paths, `std::endl`, `std::bind`; uses `base::flat_map`
- **Abseil (Google)**: `absl::flat_hash_map`, `string_view` by default, `absl::InlinedVector`
- **folly (Meta)**: `folly::small_vector`, `folly::Function` (move-only)
- **CppCoreGuidelines**: `span` over (ptr, size), RAII for threads, `noexcept` move operations

### High-Load Focus

1. **Allocations**: every heap allocation is a potential cache miss + allocator contention
2. **Cache locality**: node-based containers (list, map, set) scatter data across the heap
3. **Atomic operations**: `shared_ptr` refcount bouncing between cores
4. **Virtual dispatch**: indirect branch prediction misses in tight loops
5. **Exceptions**: unwind table overhead, unpredictable control flow
6. **Locale contention**: global locale mutex in `stoi`/`to_string`/`stringstream`
7. **RTTI overhead**: `dynamic_cast` traverses vtable hierarchy at runtime
8. **Move semantics**: missing `noexcept` on move operations forces `vector` to copy on reallocation

## Architecture

```
src/
├── HlTidyModule.cpp          # Module registration for all checks
├── HlTidyModule.h
├── utils/
│   ├── CppStandardUtils.h    # C++ standard detection from LangOptions
│   └── DiagnosticHelper.h    # Diagnostic message formatting utilities
└── checks/
    ├── AvoidStd*Check.*      # "Avoid X" type checks
    └── Prefer*Check.*        # "Prefer Y" type checks
```

Each check is a standalone class inheriting from `clang::tidy::ClangTidyCheck`. Checks use AST matchers to find problematic patterns and emit diagnostics with replacement suggestions.

## Author

**Aleksandr Loshkarev**

## License

MIT
