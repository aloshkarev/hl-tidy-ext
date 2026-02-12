//===--- PreferFormatCheck.h - hl-modernize-prefer-format -----*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::stringstream, sprintf, snprintf for string formatting
// and recommends std::format (C++20) / std::print (C++23).
//
// std::format advantages:
//   - Type-safe (no format-string vulnerabilities)
//   - Faster than stringstream (no virtual dispatch, no locale by default)
//   - Compile-time format string validation (C++20)
//   - No buffer overflow risk (unlike sprintf)
//
// References:
//   - P0645R10 (std::format)
//   - P2093R14 (std::print, C++23)
//   - {fmt} library benchmarks
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_FORMAT_CHECK_H
#define HL_TIDY_CHECKS_PREFER_FORMAT_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferFormatCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferFormatCheck(llvm::StringRef Name,
                    clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus20;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_FORMAT_CHECK_H
