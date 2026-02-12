//===--- PreferEraseIfCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags the erase-remove idiom and recommends std::erase_if (C++20).
//
// The traditional pattern:
//   v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
//
// Problems:
//   - Verbose and error-prone (easy to forget the second v.end())
//   - Does not work uniformly on all containers (node-based need loops)
//   - The "remove" step leaves moved-from elements in the tail
//
// std::erase_if (C++20):
//   - Single function call: std::erase_if(container, predicate)
//   - Works uniformly on vector, deque, string, list, set, map, etc.
//   - Returns the number of erased elements
//
// References:
//   - P1209R0 (uniform container erasure)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_ERASE_IF_CHECK_H
#define HL_TIDY_CHECKS_PREFER_ERASE_IF_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferEraseIfCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferEraseIfCheck(llvm::StringRef Name,
                     clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus20;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_ERASE_IF_CHECK_H
