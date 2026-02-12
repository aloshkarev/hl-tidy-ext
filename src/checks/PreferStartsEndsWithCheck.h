//===--- PreferStartsEndsWithCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags idioms that check string prefixes/suffixes in suboptimal ways
// and recommends std::string::starts_with / ends_with (C++20).
//
// Detected anti-patterns:
//   s.find(prefix) == 0          — scans the ENTIRE string, O(N)
//   s.substr(0, n) == prefix     — ALLOCATES a temporary std::string!
//   s.compare(0, n, prefix) == 0 — correct but verbose
//   s.rfind(suffix, s.size()-n)  — awkward suffix check
//
// starts_with / ends_with:
//   - O(prefix/suffix length), no allocation, clear intent
//   - Also available on std::string_view
//
// References:
//   - P0457R2 (starts_with / ends_with)
//   - Abseil StrContains / StartsWith (pre-C++20 equivalents)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_STARTS_ENDS_WITH_CHECK_H
#define HL_TIDY_CHECKS_PREFER_STARTS_ENDS_WITH_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferStartsEndsWithCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferStartsEndsWithCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_STARTS_ENDS_WITH_CHECK_H
