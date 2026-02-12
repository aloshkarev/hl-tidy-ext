//===--- PreferStartsEndsWithCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferStartsEndsWithCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferStartsEndsWithCheck::PreferStartsEndsWithCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferStartsEndsWithCheck::registerMatchers(MatchFinder *Finder) {
  // Pattern 1: s.find(x) == 0
  // Match any .find() == 0 on an object whose unqualified type has
  // a "starts_with" method (i.e., it is a string or string_view).
  Finder->addMatcher(
      binaryOperator(
          hasOperatorName("=="),
          hasEitherOperand(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(
                      hasName("find"),
                      ofClass(cxxRecordDecl(hasMethod(hasName("starts_with")))))))
                  .bind("find_call")),
          hasEitherOperand(ignoringImplicit(
              integerLiteral(equals(0)))))
          .bind("find_eq_zero"),
      this);

  // Pattern 2: s.substr(0, n) == x  (uses overloaded operator==)
  // The substr call is wrapped in MaterializeTemporaryExpr + ImplicitCastExpr,
  // so use hasDescendant to look through all wrapping layers.
  Finder->addMatcher(
      cxxOperatorCallExpr(
          hasOverloadedOperatorName("=="),
          hasDescendant(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(
                      hasName("substr"),
                      ofClass(cxxRecordDecl(hasMethod(hasName("starts_with")))))),
                  hasArgument(0, ignoringImplicit(integerLiteral(equals(0)))))
                  .bind("substr_call")))
          .bind("substr_eq"),
      this);

  // Pattern 3: s.compare(0, n, x) == 0
  Finder->addMatcher(
      binaryOperator(
          hasOperatorName("=="),
          hasEitherOperand(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(
                      hasName("compare"),
                      ofClass(cxxRecordDecl(hasMethod(hasName("starts_with")))))),
                  hasArgument(0, ignoringImplicit(integerLiteral(equals(0)))))
                  .bind("compare_call")),
          hasEitherOperand(ignoringImplicit(
              integerLiteral(equals(0)))))
          .bind("compare_eq_zero"),
      this);
}

void PreferStartsEndsWithCheck::check(
    const MatchFinder::MatchResult &Result) {

  if (Result.Nodes.getNodeAs<clang::BinaryOperator>("find_eq_zero")) {
    const auto *Call =
        Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("find_call");
    if (!Call) return;

    diag(Call->getExprLoc(),
         "s.find(x) == 0 scans the entire string to check a prefix; "
         "use s.starts_with(x) (C++20) which is O(prefix length)");

    diag(Call->getExprLoc(),
         "starts_with performs no allocation and clearly expresses "
         "intent; also works on std::string_view",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (Result.Nodes.getNodeAs<clang::CXXOperatorCallExpr>("substr_eq")) {
    const auto *Call =
        Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("substr_call");
    if (!Call) return;

    diag(Call->getExprLoc(),
         "s.substr(0, n) == x ALLOCATES a temporary string to check "
         "a prefix; use s.starts_with(x) (C++20) for zero allocation");

    diag(Call->getExprLoc(),
         "this pattern creates a heap-allocated temporary on every "
         "call; critical in hot paths with high request rates",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (Result.Nodes.getNodeAs<clang::BinaryOperator>("compare_eq_zero")) {
    const auto *Call =
        Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("compare_call");
    if (!Call) return;

    diag(Call->getExprLoc(),
         "s.compare(0, n, x) == 0 is correct but verbose; "
         "use s.starts_with(x) (C++20) for clarity");
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
