//===--- AvoidStdBindCheck.h - hl-perf-avoid-std-bind ---------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::bind which has several performance problems:
//   - Creates a type-erased wrapper (similar to std::function overhead)
//   - Cannot be inlined by the compiler in most cases
//   - Generates complex template instantiations (longer compile times)
//   - Chromium explicitly bans std::bind in favor of lambdas
//
// Lambdas are strictly superior:
//   - Zero overhead (compiler can see through and inline)
//   - Clearer intent and better error messages
//   - More flexible (auto parameters, if constexpr, etc.)
//
// References:
//   - Chromium C++ style guide: "Do not use std::bind"
//   - Abseil Tip #108: avoid std::bind
//   - Scott Meyers, Effective Modern C++ Item 34
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_STD_BIND_CHECK_H
#define HL_TIDY_CHECKS_AVOID_STD_BIND_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidStdBindCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidStdBindCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_STD_BIND_CHECK_H
