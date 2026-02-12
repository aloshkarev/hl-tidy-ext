//===--- PreferEmplaceCheck.h - hl-perf-prefer-emplace --------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags patterns where push_back / insert is called with a temporary
// constructor and recommends emplace_back / emplace for in-place
// construction, avoiding an unnecessary copy/move.
//
// Example:
//   vec.push_back(std::string("hello"));  // constructs temp, then moves
//   vec.emplace_back("hello");            // constructs in-place
//
// This matters in hot paths: the temporary triggers a constructor + move
// + destructor instead of a single constructor.
//
// References:
//   - Effective Modern C++ Item 42 (Scott Meyers)
//   - CppCoreGuidelines ES.23
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_EMPLACE_CHECK_H
#define HL_TIDY_CHECKS_PREFER_EMPLACE_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferEmplaceCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferEmplaceCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_EMPLACE_CHECK_H
