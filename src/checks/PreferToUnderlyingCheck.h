//===--- PreferToUnderlyingCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags static_cast to the underlying type of a scoped enum and recommends
// std::to_underlying() (C++23).
//
// Problem with static_cast<int>(enum_val):
//   - Fragile: if the enum's underlying type changes, the cast breaks
//   - Verbose and hides intent
//   - Easy to cast to the wrong integer type silently
//
// std::to_underlying() (C++23):
//   - Always returns the correct underlying type
//   - Self-documenting intent
//   - Compile-time evaluated, zero overhead
//
// References:
//   - P1682R3 (std::to_underlying)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_TO_UNDERLYING_CHECK_H
#define HL_TIDY_CHECKS_PREFER_TO_UNDERLYING_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferToUnderlyingCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferToUnderlyingCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_TO_UNDERLYING_CHECK_H
