//===--- PreferExpectedCheck.cpp - hl-modernize-prefer-expected -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferExpectedCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferExpectedCheck::PreferExpectedCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferExpectedCheck::registerMatchers(MatchFinder *Finder) {
  // Match functions that return std::optional — potential candidates
  // for std::expected when the "empty" state represents an error.
  Finder->addMatcher(
      functionDecl(
          returns(qualType(hasDeclaration(
              namedDecl(hasName("::std::optional"))))))
          .bind("optional_return"),
      this);

  // Match throw expressions inside non-constructor/destructor functions.
  // In high-load code, exceptions for control flow are expensive.
  Finder->addMatcher(
      cxxThrowExpr(
          hasAncestor(functionDecl(
              unless(cxxConstructorDecl()),
              unless(cxxDestructorDecl()))
              .bind("throwing_func")))
          .bind("throw_expr"),
      this);
}

void PreferExpectedCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *OptFunc =
          Result.Nodes.getNodeAs<clang::FunctionDecl>("optional_return")) {
    diag(OptFunc->getLocation(),
         "function returns std::optional but may benefit from "
         "std::expected<T, E> (C++23) to propagate error information");

    diag(OptFunc->getLocation(),
         "std::expected carries both the success value and a typed error, "
         "enabling richer error handling without exceptions",
         clang::DiagnosticIDs::Note);

    diag(OptFunc->getLocation(),
         "use monadic operations (.and_then, .or_else, .transform) "
         "for composable error handling pipelines",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Throw =
          Result.Nodes.getNodeAs<clang::CXXThrowExpr>("throw_expr")) {
    diag(Throw->getThrowLoc(),
         "throw expression in regular function; exceptions have high "
         "overhead for expected error conditions — consider returning "
         "std::expected<T, E> (C++23) instead");

    diag(Throw->getThrowLoc(),
         "exception unwinding traverses the call stack and involves "
         "RTTI; std::expected is a zero-overhead value type",
         clang::DiagnosticIDs::Note);

    diag(Throw->getThrowLoc(),
         "reserve exceptions for truly exceptional/unrecoverable "
         "conditions (out of memory, invariant violations)",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
