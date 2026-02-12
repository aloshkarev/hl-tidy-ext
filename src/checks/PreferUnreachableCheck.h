//===--- PreferUnreachableCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags compiler-specific unreachable hints and recommends the portable
// std::unreachable() (C++23).
//
// Detected patterns:
//   __builtin_unreachable()        -- GCC/Clang only
//   __assume(false)                -- MSVC only
//   assert(false); abort();        -- runtime cost, not optimizable
//
// std::unreachable() (C++23):
//   - Portable across all conforming compilers
//   - Allows the optimizer to assume the code path is never taken
//   - Undefined behavior if reached (same as __builtin_unreachable)
//
// References:
//   - P0627R6 (std::unreachable)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_UNREACHABLE_CHECK_H
#define HL_TIDY_CHECKS_PREFER_UNREACHABLE_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferUnreachableCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferUnreachableCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_UNREACHABLE_CHECK_H
