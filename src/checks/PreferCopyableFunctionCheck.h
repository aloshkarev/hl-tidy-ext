//===--- PreferCopyableFunctionCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// When C++26 is available, flags std::function usage and recommends
// std::copyable_function which is a stricter, better-designed replacement.
//
// Problems with std::function:
//   - Callable's const-correctness is not enforced (operator() is const
//     but the stored callable may be mutated via mutable lambdas)
//   - No noexcept qualifier support
//   - Permits implicit conversions that may silently discard qualifiers
//
// std::copyable_function (C++26):
//   - Supports const, volatile, ref, and noexcept qualifiers on operator()
//   - Enforces that operator() const only calls const callables
//   - Better SBO (small-buffer optimization) due to simpler type erasure
//   - Drop-in replacement for std::function where copies are needed
//
// References:
//   - P2548R6 (std::copyable_function)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_COPYABLE_FUNCTION_CHECK_H
#define HL_TIDY_CHECKS_PREFER_COPYABLE_FUNCTION_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferCopyableFunctionCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferCopyableFunctionCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_COPYABLE_FUNCTION_CHECK_H
