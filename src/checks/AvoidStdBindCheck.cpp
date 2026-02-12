//===--- AvoidStdBindCheck.cpp - hl-perf-avoid-std-bind -------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidStdBindCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidStdBindCheck::AvoidStdBindCheck(llvm::StringRef Name,
                                     clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidStdBindCheck::registerMatchers(MatchFinder *Finder) {
  // Match calls to std::bind.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::bind"))))
          .bind("bind_call"),
      this);

  // Match calls to std::bind_front (C++20) — less problematic but still
  // prefer lambdas for consistency and inlining.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::bind_front"))))
          .bind("bind_front_call"),
      this);

  // Match calls to std::bind_back (C++23).
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::bind_back"))))
          .bind("bind_back_call"),
      this);
}

void AvoidStdBindCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *Bind =
          Result.Nodes.getNodeAs<clang::CallExpr>("bind_call")) {
    diag(Bind->getExprLoc(),
         "std::bind creates a type-erased wrapper that prevents "
         "inlining; use a lambda instead (banned in Chromium)");

    diag(Bind->getExprLoc(),
         "lambdas are zero-overhead, inlineable, and produce clearer "
         "error messages; prefer [captures](params){ body }",
         clang::DiagnosticIDs::Note);

    diag(Bind->getExprLoc(),
         "std::bind also interacts poorly with overloaded functions "
         "and default arguments — lambdas handle both correctly",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *BF =
          Result.Nodes.getNodeAs<clang::CallExpr>("bind_front_call")) {
    diag(BF->getExprLoc(),
         "std::bind_front (C++20) is better than std::bind but "
         "lambdas are still preferred for inlining and clarity");

    diag(BF->getExprLoc(),
         "replace with a lambda: [captured_args](auto&&... rest){ "
         "return fn(captured_args, std::forward<decltype(rest)>(rest)...); }",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *BB =
          Result.Nodes.getNodeAs<clang::CallExpr>("bind_back_call")) {
    diag(BB->getExprLoc(),
         "std::bind_back (C++23) is better than std::bind but "
         "lambdas are still preferred for inlining and clarity");
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
