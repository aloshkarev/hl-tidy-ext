//===--- PreferStringViewCheck.h - hl-perf-prefer-string-view --*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags function parameters declared as 'const std::string&' where
// std::string_view (C++17) would avoid unnecessary allocations when
// called with string literals or substrings.
//
// References:
//   - Abseil Tip #1: string_view
//   - Google C++ Style Guide recommends string_view for read-only params
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_STRING_VIEW_CHECK_H
#define HL_TIDY_CHECKS_PREFER_STRING_VIEW_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferStringViewCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferStringViewCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_STRING_VIEW_CHECK_H
