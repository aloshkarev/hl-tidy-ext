//===--- PreferFunctionRefCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferFunctionRefCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferFunctionRefCheck::PreferFunctionRefCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferFunctionRefCheck::registerMatchers(MatchFinder *Finder) {
  // Match function parameters of type std::function (by value or const ref).
  // These are prime candidates for std::function_ref when the callable
  // does not outlive the function call.
  Finder->addMatcher(
      parmVarDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("func_param"),
      this);

  // Also match const std::function& parameters.
  Finder->addMatcher(
      parmVarDecl(hasType(references(qualType(
          isConstQualified(),
          hasDeclaration(namedDecl(hasName("::std::function")))))))
          .bind("func_cref_param"),
      this);
}

void PreferFunctionRefCheck::check(
    const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;

  if (const auto *P =
          Result.Nodes.getNodeAs<clang::ParmVarDecl>("func_param")) {
    Loc = P->getLocation();
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>(
                     "func_cref_param")) {
    Loc = P->getLocation();
  } else {
    return;
  }

  diag(Loc,
       "std::function parameter can likely be replaced with "
       "std::function_ref (C++26) for non-owning callable references");

  diag(Loc,
       "std::function_ref has zero overhead: no heap allocation, "
       "no type-erasure copy/move operations, typically just 2 pointers",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "use std::function_ref when the callable does not need to outlive "
       "the function call; use std::move_only_function (C++23) for "
       "ownership transfer",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
