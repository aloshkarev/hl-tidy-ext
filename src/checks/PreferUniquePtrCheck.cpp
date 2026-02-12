//===--- PreferUniquePtrCheck.cpp - hl-perf-prefer-unique-ptr --*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferUniquePtrCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferUniquePtrCheck::PreferUniquePtrCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferUniquePtrCheck::registerMatchers(MatchFinder *Finder) {
  // Flag std::make_shared calls — these are clear construction sites.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::make_shared"))))
          .bind("make_shared"),
      this);

  // Flag field declarations with shared_ptr type.
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::shared_ptr"))))
          .bind("shared_field"),
      this);

  // Flag shared_ptr parameters passed by value (copy = atomic inc/dec).
  Finder->addMatcher(
      parmVarDecl(
          hasType(namedDecl(hasName("::std::shared_ptr"))),
          unless(hasType(references(qualType()))))
          .bind("shared_param_by_value"),
      this);
}

void PreferUniquePtrCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *MS =
          Result.Nodes.getNodeAs<clang::CallExpr>("make_shared")) {
    diag(MS->getExprLoc(),
         "std::make_shared implies shared ownership with atomic "
         "reference counting; prefer std::make_unique if ownership "
         "is not shared");

    diag(MS->getExprLoc(),
         "std::unique_ptr has zero overhead over raw pointers and "
         "no atomic operations; use std::shared_ptr only when "
         "multiple owners are genuinely needed",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *SF =
          Result.Nodes.getNodeAs<clang::FieldDecl>("shared_field")) {
    diag(SF->getLocation(),
         "std::shared_ptr class member: atomic reference counting causes "
         "cache-line bouncing in multi-threaded code; ensure shared "
         "ownership is truly required");

    diag(SF->getLocation(),
         "if single ownership suffices, use std::unique_ptr; "
         "if borrowing, use a raw pointer or std::observer_ptr",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *SP = Result.Nodes.getNodeAs<clang::ParmVarDecl>(
          "shared_param_by_value")) {
    diag(SP->getLocation(),
         "std::shared_ptr passed by value triggers atomic "
         "increment/decrement of the reference count");

    diag(SP->getLocation(),
         "pass by 'const std::shared_ptr&' if you only need to read, "
         "or pass the underlying 'T&'/'T*' if ownership is not transferred",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
