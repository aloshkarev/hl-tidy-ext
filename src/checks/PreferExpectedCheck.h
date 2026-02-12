//===--- PreferExpectedCheck.h - hl-modernize-prefer-expected --*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags patterns where exceptions are used for expected error conditions
// and recommends std::expected (C++23) for value-or-error return types.
//
// Also flags std::optional used as an error channel (losing error info)
// and suggests std::expected as a richer alternative.
//
// std::expected advantages:
//   - Zero heap allocation (inline storage)
//   - No exception unwinding cost
//   - Explicit error handling (no silent ignoring)
//   - Composable with monadic operations (.and_then, .or_else, .transform)
//
// References:
//   - P0323R12 (std::expected)
//   - Herb Sutter "Zero-overhead deterministic exceptions" (P0709)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_EXPECTED_CHECK_H
#define HL_TIDY_CHECKS_PREFER_EXPECTED_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferExpectedCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferExpectedCheck(llvm::StringRef Name,
                      clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus23;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_EXPECTED_CHECK_H
