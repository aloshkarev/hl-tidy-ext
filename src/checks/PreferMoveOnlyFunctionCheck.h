//===--- PreferMoveOnlyFunctionCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// When C++23 is available and std::function is used for callbacks that
// are only moved (not copied), suggests std::move_only_function which:
//   - Does not require the callable to be copyable
//   - Can use SBO more aggressively (no copy vtable entry needed)
//   - Better models ownership-transfer semantics
//
// References:
//   - P0288R9 (std::move_only_function)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_MOVE_ONLY_FUNCTION_CHECK_H
#define HL_TIDY_CHECKS_PREFER_MOVE_ONLY_FUNCTION_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferMoveOnlyFunctionCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferMoveOnlyFunctionCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_MOVE_ONLY_FUNCTION_CHECK_H
