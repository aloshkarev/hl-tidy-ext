//===--- AvoidDynamicCastCheck.h - hl-perf-avoid-dynamic-cast --*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of dynamic_cast which requires RTTI and performs a runtime
// vtable hierarchy traversal.  In high-load services:
//   - RTTI inflates binary size (type_info for every polymorphic class)
//   - dynamic_cast is O(depth) in the class hierarchy
//   - Chromium, LLVM, and many game engines build with -fno-rtti
//
// Recommendations:
//   - static_cast with an enum/tag-based type field
//   - Visitor pattern / std::variant (closed type set)
//   - CRTP for compile-time polymorphism
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_DYNAMIC_CAST_CHECK_H
#define HL_TIDY_CHECKS_AVOID_DYNAMIC_CAST_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidDynamicCastCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidDynamicCastCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_DYNAMIC_CAST_CHECK_H
