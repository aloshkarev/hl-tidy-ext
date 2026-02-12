//===--- PreferVectorOverListCheck.h - hl-perf-prefer-vector ---*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::list and std::forward_list which have poor cache
// locality due to node-based allocation.  std::vector (contiguous memory)
// is almost always faster for iteration, insertion at end, and even
// random insertion for small-to-medium sizes thanks to cache effects.
//
// Also flags std::deque when std::vector would suffice.
//
// References:
//   - Bjarne Stroustrup "Why you should avoid linked lists"
//   - Chromium bans std::list in performance-sensitive code
//   - Mike Acton "Data-Oriented Design and C++"
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_VECTOR_OVER_LIST_CHECK_H
#define HL_TIDY_CHECKS_PREFER_VECTOR_OVER_LIST_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferVectorOverListCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferVectorOverListCheck(llvm::StringRef Name,
                            clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_VECTOR_OVER_LIST_CHECK_H
