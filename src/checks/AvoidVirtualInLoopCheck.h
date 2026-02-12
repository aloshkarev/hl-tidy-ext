//===--- AvoidVirtualInLoopCheck.h - hl-perf-avoid-virtual-in-loop *- C++ *-
// Author: Aleksandr Loshkarev
//
// Flags virtual method calls inside tight loops (for, while, do, range-for).
// Virtual dispatch involves:
//   - Indirect function call through the vtable pointer
//   - Branch predictor miss on every call (if the target varies)
//   - Prevents inlining (the compiler cannot see the target)
//
// In high-load hot loops this can cause 2-10x slowdown compared to direct
// or template-based dispatch.
//
// Recommendations:
//   - CRTP (Curiously Recurring Template Pattern)
//   - if constexpr with type tags (C++17)
//   - std::variant + std::visit (can be devirtualized by the compiler)
//   - Manual devirtualization: cache the function pointer before the loop
//
// References:
//   - Mike Acton "Data-Oriented Design and C++" (CppCon 2014)
//   - Chandler Carruth "Efficiency with Algorithms" (CppCon 2014)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_VIRTUAL_IN_LOOP_CHECK_H
#define HL_TIDY_CHECKS_AVOID_VIRTUAL_IN_LOOP_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidVirtualInLoopCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidVirtualInLoopCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_VIRTUAL_IN_LOOP_CHECK_H
