//===--- PreferNoexceptMoveCheck.h - hl-perf-prefer-noexcept-move -*- C++ *-//
// Author: Aleksandr Loshkarev
//
// Flags move constructors and move assignment operators that are not
// declared noexcept.  Without noexcept:
//   - std::vector::push_back and resize COPY instead of MOVE elements
//     during reallocation (strong exception guarantee requirement)
//   - std::swap may fall back to copies
//   - Move-based algorithms (std::sort, etc.) lose optimization
//
// This is one of the most impactful performance issues in C++: a single
// missing noexcept on a move constructor can turn O(N) reallocation
// into O(N * copy_cost).
//
// References:
//   - Effective Modern C++ Item 14 (Scott Meyers)
//   - CppCoreGuidelines C.66: Make move operations noexcept
//   - Chromium requires noexcept on move operations
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_NOEXCEPT_MOVE_CHECK_H
#define HL_TIDY_CHECKS_PREFER_NOEXCEPT_MOVE_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferNoexceptMoveCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferNoexceptMoveCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_NOEXCEPT_MOVE_CHECK_H
