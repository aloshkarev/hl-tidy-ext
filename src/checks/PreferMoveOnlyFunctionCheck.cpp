//===--- PreferMoveOnlyFunctionCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferMoveOnlyFunctionCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferMoveOnlyFunctionCheck::PreferMoveOnlyFunctionCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferMoveOnlyFunctionCheck::registerMatchers(MatchFinder *Finder) {
  // Match std::function fields (often used for storing callbacks).
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("func_field"),
      this);

  // Match std::function parameters taken by value (ownership transfer).
  Finder->addMatcher(
      parmVarDecl(
          hasType(namedDecl(hasName("::std::function"))),
          unless(hasType(references(qualType()))))
          .bind("func_param_val"),
      this);

  // Match std::function parameters taken by rvalue reference.
  Finder->addMatcher(
      parmVarDecl(
          hasType(rValueReferenceType(
              pointee(hasDeclaration(
                  namedDecl(hasName("::std::function")))))))
          .bind("func_param_rval"),
      this);
}

void PreferMoveOnlyFunctionCheck::check(
    const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;
  llvm::StringRef Kind;

  if (const auto *F =
          Result.Nodes.getNodeAs<clang::FieldDecl>("func_field")) {
    Loc = F->getLocation();
    Kind = "field";
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>(
                     "func_param_val")) {
    Loc = P->getLocation();
    Kind = "by-value parameter";
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>(
                     "func_param_rval")) {
    Loc = P->getLocation();
    Kind = "rvalue-reference parameter";
  } else {
    return;
  }

  diag(Loc,
       "std::function %0 can likely be replaced with "
       "std::move_only_function (C++23) for move-only callbacks")
      << Kind;

  diag(Loc,
       "std::move_only_function does not require the callable to be "
       "copyable, enabling more aggressive small-buffer optimization "
       "and eliminating unnecessary copy overhead",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "std::move_only_function also supports const, noexcept, and "
       "ref-qualified call operators for stronger API contracts",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
