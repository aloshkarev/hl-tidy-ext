//===--- PreferEmplaceCheck.cpp - hl-perf-prefer-emplace ------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferEmplaceCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferEmplaceCheck::PreferEmplaceCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferEmplaceCheck::registerMatchers(MatchFinder *Finder) {
  // Match push_back called with a CXXConstructExpr (explicit temporary).
  // e.g. vec.push_back(MyType(arg1, arg2))
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(hasName("push_back"))),
          hasArgument(
              0,
              cxxConstructExpr(
                  unless(hasDeclaration(
                      cxxConstructorDecl(isCopyConstructor()))),
                  unless(hasDeclaration(
                      cxxConstructorDecl(isMoveConstructor()))))
                  .bind("ctor")))
          .bind("push_back_ctor"),
      this);

  // Match insert called with a temporary.
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(hasName("insert"))),
          hasAnyArgument(
              cxxConstructExpr(
                  unless(hasDeclaration(
                      cxxConstructorDecl(isCopyConstructor()))),
                  unless(hasDeclaration(
                      cxxConstructorDecl(isMoveConstructor()))))
                  .bind("insert_ctor")))
          .bind("insert_call"),
      this);
}

void PreferEmplaceCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *PB =
          Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>(
              "push_back_ctor")) {
    diag(PB->getExprLoc(),
         "push_back with an explicit constructor creates an unnecessary "
         "temporary; use emplace_back for in-place construction");

    diag(PB->getExprLoc(),
         "emplace_back forwards arguments directly to the constructor, "
         "avoiding the temporary's construction + move + destruction",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Ins =
          Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>(
              "insert_call")) {
    diag(Ins->getExprLoc(),
         "insert with an explicit constructor creates an unnecessary "
         "temporary; use emplace for in-place construction");

    diag(Ins->getExprLoc(),
         "emplace forwards arguments directly to the constructor, "
         "avoiding the temporary object overhead",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
