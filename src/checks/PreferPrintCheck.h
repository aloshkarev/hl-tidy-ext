//===--- PreferPrintCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags printf-family functions and cout-based output in C++23 code and
// recommends std::print / std::println.
//
// Problems with printf:
//   - Not type-safe (format string vulnerabilities, UB on mismatch)
//   - No compile-time format validation (until compilers added warnings)
//   - Cannot handle user-defined types
//
// Problems with cout:
//   - Global mutex contention (sync_with_stdio)
//   - Virtual dispatch in streambuf
//   - Stateful formatting (width, fill, flags persist)
//   - Verbose chained << operators
//
// std::print / std::println (C++23):
//   - Type-safe with compile-time format string validation
//   - No global mutex (writes directly to file descriptor)
//   - Supports user-defined types via std::formatter
//   - No trailing flush (unlike cout with endl)
//
// References:
//   - P2093R14 (std::print)
//   - P2539R4 (std::println)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_PRINT_CHECK_H
#define HL_TIDY_CHECKS_PREFER_PRINT_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferPrintCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferPrintCheck(llvm::StringRef Name,
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

#endif // HL_TIDY_CHECKS_PREFER_PRINT_CHECK_H
