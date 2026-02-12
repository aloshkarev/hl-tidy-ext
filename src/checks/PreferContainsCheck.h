//===--- PreferContainsCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags idioms checking key/element existence in containers/strings in
// suboptimal or verbose ways. Recommends .contains() (C++20).
//
// Detected anti-patterns:
//   m.find(k) != m.end()   -- returns an iterator that is just discarded
//   m.count(k) > 0         -- count may traverse duplicates (O(N+log N))
//   s.find(x) != npos      -- verbose for existence check
//
// .contains() (C++20):
//   - Clearer intent (boolean result)
//   - Potentially faster (can short-circuit)
//   - Available on all associative/unordered containers and strings
//
// References:
//   - P0458R2 (contains for associative containers)
//   - P1679R3 (contains for basic_string / basic_string_view)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_CONTAINS_CHECK_H
#define HL_TIDY_CHECKS_PREFER_CONTAINS_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferContainsCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferContainsCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_CONTAINS_CHECK_H
