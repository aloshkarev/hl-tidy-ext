//===--- PreferUnreachableCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferUnreachableCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferUnreachableCheck::PreferUnreachableCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferUnreachableCheck::registerMatchers(MatchFinder *Finder) {
  // Match __builtin_unreachable() calls.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("__builtin_unreachable"))))
          .bind("builtin_unreachable"),
      this);

  // Match __assume(false) -- MSVC intrinsic (sometimes used in cross-platform).
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("__assume"))))
          .bind("assume_false"),
      this);
}

void PreferUnreachableCheck::check(
    const MatchFinder::MatchResult &Result) {
  if (const auto *Call =
          Result.Nodes.getNodeAs<clang::CallExpr>("builtin_unreachable")) {
    diag(Call->getExprLoc(),
         "__builtin_unreachable() is a GCC/Clang extension; "
         "use std::unreachable() (C++23) for portable code");

    diag(Call->getExprLoc(),
         "std::unreachable() provides the same optimization hint to "
         "the compiler while being portable across all C++23 compilers",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Call =
          Result.Nodes.getNodeAs<clang::CallExpr>("assume_false")) {
    diag(Call->getExprLoc(),
         "__assume(false) is an MSVC extension; "
         "use std::unreachable() (C++23) for portable code");
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
