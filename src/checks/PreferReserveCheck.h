//===--- PreferReserveCheck.h - hl-perf-prefer-reserve ---------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags patterns where std::vector::push_back / emplace_back is called
// inside a loop without a prior reserve() call.  Repeated reallocation
// is one of the most common performance pitfalls in highload services.
//
// References:
//   - Effective STL Item 14 (Scott Meyers)
//   - Chromium base/containers guidelines
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_RESERVE_CHECK_H
#define HL_TIDY_CHECKS_PREFER_RESERVE_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferReserveCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferReserveCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_RESERVE_CHECK_H
