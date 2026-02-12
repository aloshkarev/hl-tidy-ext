//===--- PreferFromCharsCheck.h - hl-perf-prefer-from-chars ----*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags calls to std::stoi, std::stol, std::stof, std::stod, std::stoul,
// std::stoull, std::to_string and std::stringstream-based number conversions.
//
// std::from_chars / std::to_chars (C++17) are:
//   - Locale-independent (no global locale lock contention)
//   - Exception-free (error reported via return code)
//   - No heap allocations
//   - Up to 10-20x faster than stringstream
//
// References:
//   - P0067R5 (std::from_chars / std::to_chars)
//   - Chromium prefers non-throwing number parsing
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_FROM_CHARS_CHECK_H
#define HL_TIDY_CHECKS_PREFER_FROM_CHARS_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferFromCharsCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferFromCharsCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_FROM_CHARS_CHECK_H
