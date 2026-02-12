//===--- AvoidCoutCerrCheck.h - hl-perf-avoid-cout-cerr -------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags direct usage of std::cout and std::cerr in production code.
// These global streams have:
//   - A global mutex (sync_with_stdio) causing contention in MT code
//   - Formatting overhead via virtual dispatch in the streambuf
//   - No log-level filtering, rotation, or structured output
//
// Recommendations:
//   - spdlog (header-only, zero-alloc formatting, async mode)
//   - glog (Google's logging library)
//   - Custom lock-free logging with ring buffers
//   - fmt::print for non-logging output
//
// References:
//   - Chromium uses LOG() macros, not std::cout
//   - Google style: production code should use logging frameworks
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_AVOID_COUT_CERR_CHECK_H
#define HL_TIDY_CHECKS_AVOID_COUT_CERR_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class AvoidCoutCerrCheck : public clang::tidy::ClangTidyCheck {
public:
  AvoidCoutCerrCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_AVOID_COUT_CERR_CHECK_H
