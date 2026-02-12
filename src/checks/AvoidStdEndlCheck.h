//===--- AvoidStdEndlCheck.h - hl-perf-avoid-std-endl ----------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::endl which forces a stream flush after every line.
// In high-throughput logging / output this is devastating for performance.
//
// Replacement: '\n' character literal.
//
// References:
//   - Chromium style guide bans std::endl
//   - Google C++ style: prefer '\n'
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_STD_ENDL_CHECK_H
#define HL_TIDY_CHECKS_AVOID_STD_ENDL_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidStdEndlCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidStdEndlCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_STD_ENDL_CHECK_H
