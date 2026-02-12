//===--- PreferFlatContainersCheck.h - hl-modernize-prefer-flat -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::map / std::set / std::multimap / std::multiset
// (tree-based, node-allocated) and recommends:
//   C++17 : sorted std::vector + std::lower_bound, or boost::flat_map
//   C++23+: std::flat_map, std::flat_set (contiguous, cache-friendly)
//
// Tree-based containers have:
//   - O(log N) with high constant factor due to cache misses
//   - Per-node heap allocation
//   - Pointer chasing (every lookup traverses heap-allocated nodes)
//
// Flat containers store keys/values in contiguous vectors:
//   - Cache-friendly iteration and lookup
//   - Fewer allocations
//   - Better for read-heavy workloads (typical in high-load services)
//
// References:
//   - Chromium uses base::flat_map / base::flat_set
//   - Boost.Container flat_map/flat_set
//   - P0429R9 (std::flat_map, C++23)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_FLAT_CONTAINERS_CHECK_H
#define HL_TIDY_CHECKS_PREFER_FLAT_CONTAINERS_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferFlatContainersCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferFlatContainersCheck(llvm::StringRef Name,
                            clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus17;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_FLAT_CONTAINERS_CHECK_H
