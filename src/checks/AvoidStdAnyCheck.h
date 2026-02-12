//===--- AvoidStdAnyCheck.h - hl-perf-avoid-std-any -----------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::any which performs heap allocation for non-trivial
// types and requires RTTI for type identification.
//
// Recommendations:
//   - std::variant for a closed set of types (zero overhead, no heap alloc)
//   - Template-based polymorphism for open type sets
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_STD_ANY_CHECK_H
#define HL_TIDY_CHECKS_AVOID_STD_ANY_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidStdAnyCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidStdAnyCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_STD_ANY_CHECK_H
