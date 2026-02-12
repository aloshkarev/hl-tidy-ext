//===--- PreferSpanCheck.h - hl-modernize-prefer-span ---------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags function parameters that take (pointer, size) pairs and recommends
// std::span (C++20) which provides bounds-safe, zero-overhead contiguous
// range access.
//
// Also flags functions taking 'const std::vector<T>&' when they only
// need read access to contiguous data — std::span<const T> is more general.
//
// References:
//   - P0122R7 (std::span)
//   - CppCoreGuidelines: use span instead of (pointer, count) pairs
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_SPAN_CHECK_H
#define HL_TIDY_CHECKS_PREFER_SPAN_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferSpanCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferSpanCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_SPAN_CHECK_H
