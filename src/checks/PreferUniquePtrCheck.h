//===--- PreferUniquePtrCheck.h - hl-perf-prefer-unique-ptr ----*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::shared_ptr where std::unique_ptr might suffice.
// std::shared_ptr has:
//   - Atomic reference counting overhead (cache-line bouncing in MT)
//   - Separate control block heap allocation
//   - Larger sizeof (two pointers vs one)
//
// Heuristic: flags local shared_ptr that are never copied.
//
// References:
//   - Herb Sutter "GotW #89: Smart Pointers"
//   - Chromium prefers unique_ptr by default
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_UNIQUE_PTR_CHECK_H
#define HL_TIDY_CHECKS_PREFER_UNIQUE_PTR_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferUniquePtrCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferUniquePtrCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_UNIQUE_PTR_CHECK_H
