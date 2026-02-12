//===--- PreferPrintCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferPrintCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferPrintCheck::PreferPrintCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferPrintCheck::registerMatchers(MatchFinder *Finder) {
  // Match printf / fprintf / puts / fputs calls.
  Finder->addMatcher(
      callExpr(callee(functionDecl(
                   hasAnyName("::printf", "printf",
                              "::puts", "puts"))))
          .bind("printf_call"),
      this);

  Finder->addMatcher(
      callExpr(callee(functionDecl(
                   hasAnyName("::fprintf", "fprintf",
                              "::fputs", "fputs"))))
          .bind("fprintf_call"),
      this);

  // Match cout << ... patterns (the << operator on cout).
  Finder->addMatcher(
      cxxOperatorCallExpr(
          hasOverloadedOperatorName("<<"),
          hasArgument(0,
              declRefExpr(to(namedDecl(
                  hasAnyName("::std::cout", "::std::cerr"))))))
          .bind("cout_shift"),
      this);
}

void PreferPrintCheck::check(
    const MatchFinder::MatchResult &Result) {

  if (const auto *Call =
          Result.Nodes.getNodeAs<clang::CallExpr>("printf_call")) {
    diag(Call->getExprLoc(),
         "printf/puts is not type-safe; use std::print/std::println "
         "(C++23) with compile-time format validation");

    diag(Call->getExprLoc(),
         "std::print writes directly without global mutex contention "
         "and supports user-defined types via std::formatter",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Call =
          Result.Nodes.getNodeAs<clang::CallExpr>("fprintf_call")) {
    diag(Call->getExprLoc(),
         "fprintf/fputs is not type-safe; use "
         "std::print(stream, fmt, args...) (C++23)");
    return;
  }

  if (const auto *Call =
          Result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("cout_shift")) {
    diag(Call->getExprLoc(),
         "cout/cerr << chain has global mutex contention and stateful "
         "formatting; use std::print/std::println (C++23)");

    diag(Call->getExprLoc(),
         "std::println handles newline automatically without flush "
         "overhead, and formatting is localized to each call",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
