//===--- AvoidStdRegexCheck.h - hl-perf-avoid-std-regex ---------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::regex / std::basic_regex which are notoriously slow
// due to dynamic compilation of the pattern at runtime and heavy heap
// allocations.
//
// Recommendations:
//   - Compile-time regex libraries (CTRE by Hana Dusíková)
//   - RE2 (Google) for runtime patterns with linear-time guarantees
//   - Hand-written parsers for simple patterns
//
// References:
//   - Chromium explicitly bans <regex>
//   - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1433r0.pdf
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_STD_REGEX_CHECK_H
#define HL_TIDY_CHECKS_AVOID_STD_REGEX_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidStdRegexCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidStdRegexCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_STD_REGEX_CHECK_H
