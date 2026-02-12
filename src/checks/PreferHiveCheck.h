//===--- PreferHiveCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// When C++26 is available, flags std::list and std::forward_list usage
// and recommends std::hive as a cache-friendly alternative that still
// provides pointer/iterator stability.
//
// std::list problems:
//   - Per-node heap allocation
//   - Pointer chasing on traversal (cache-hostile)
//   - 2 pointers overhead per node (forward/back)
//
// std::hive (C++26) advantages:
//   - Colony-based allocation: elements stored in contiguous blocks
//   - Stable pointers and iterators (elements never move)
//   - O(1) insert and erase (skipfield-based)
//   - Cache-friendly iteration (contiguous blocks, skip empty slots)
//   - 5-20x faster iteration than std::list in benchmarks
//
// References:
//   - P0447R26 (std::hive, formerly std::colony)
//   - plf::colony benchmarks
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_HIVE_CHECK_H
#define HL_TIDY_CHECKS_PREFER_HIVE_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferHiveCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferHiveCheck(llvm::StringRef Name,
                  clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus26;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_HIVE_CHECK_H
