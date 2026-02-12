//===--- AvoidStdFunctionCheck.h - hl-perf-avoid-std-function ----*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::function which causes heap allocation and type-erasure
// overhead.  Provides standard-aware replacement suggestions:
//   C++17 : template parameters, function pointers
//   C++20 : template parameters, function pointers
//   C++23 : std::move_only_function (when callback is not copied)
//   C++26 : std::function_ref (for non-owning references)
//
// References:
//   - Chromium C++ style: bans std::function in perf-critical code
//   - CppCon 2019 "There Are No Zero-cost Abstractions" — Chandler Carruth
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_STD_FUNCTION_CHECK_H
#define HL_TIDY_CHECKS_AVOID_STD_FUNCTION_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidStdFunctionCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidStdFunctionCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_STD_FUNCTION_CHECK_H
