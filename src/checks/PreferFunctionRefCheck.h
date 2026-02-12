//===--- PreferFunctionRefCheck.h - hl-modernize-prefer-function-ref *- C++ *-//
// Author: Aleksandr Loshkarev
//
// When C++26 is available, flags std::function used as function parameters
// where std::function_ref would be more appropriate (non-owning, zero-overhead
// type-erased callable reference).
//
// std::function_ref advantages over std::function:
//   - No heap allocation (always uses SBO-like inline storage)
//   - No atomic reference counting
//   - Lighter weight (typically 2 pointers)
//   - Perfect for callback parameters that don't outlive the call
//
// References:
//   - P0792R14 (std::function_ref)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_FUNCTION_REF_CHECK_H
#define HL_TIDY_CHECKS_PREFER_FUNCTION_REF_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferFunctionRefCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferFunctionRefCheck(llvm::StringRef Name,
                         clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus26;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_FUNCTION_REF_CHECK_H
