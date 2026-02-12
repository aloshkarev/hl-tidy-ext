//===--- PreferJthreadCheck.h - hl-modernize-prefer-jthread ---*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags usage of std::thread and recommends std::jthread (C++20) which:
//   - Automatically joins on destruction (RAII, no resource leak)
//   - Supports cooperative cancellation via std::stop_token
//   - Eliminates common bugs: dangling threads, double-join, etc.
//
// Only active when the translation unit is compiled with C++20 or later.
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_JTHREAD_CHECK_H
#define HL_TIDY_CHECKS_PREFER_JTHREAD_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferJthreadCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferJthreadCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_JTHREAD_CHECK_H
