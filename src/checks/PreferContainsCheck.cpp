//===--- PreferContainsCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferContainsCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferContainsCheck::PreferContainsCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

/// Walk parents to check if the given find() call is compared with end().
static bool isComparedWithEnd(clang::ASTContext &Ctx,
                              const clang::CXXMemberCallExpr *FindCall) {
  auto Parents = Ctx.getParents(*FindCall);
  // Walk up through implicit casts, materialize, etc.
  while (!Parents.empty()) {
    const auto &P = Parents[0];
    // Check for CXXOperatorCallExpr with == or !=.
    if (const auto *Op = P.get<clang::CXXOperatorCallExpr>()) {
      auto OpKind = Op->getOperator();
      if (OpKind == clang::OO_ExclaimEqual ||
          OpKind == clang::OO_EqualEqual) {
        // Check if any other argument is an end() call.
        for (unsigned i = 0; i < Op->getNumArgs(); ++i) {
          const auto *Arg = Op->getArg(i)->IgnoreImplicit();
          if (const auto *MC = llvm::dyn_cast<clang::CXXMemberCallExpr>(Arg)) {
            if (const auto *Method = MC->getMethodDecl()) {
              if (Method->getName() == "end")
                return true;
            }
          }
        }
      }
      return false;
    }
    // Check for CXXRewrittenBinaryOperator.
    if (P.get<clang::CXXRewrittenBinaryOperator>())
      return true; // contains() pattern detected at rewritten level
    // Check for BinaryOperator (built-in != on iterators in some cases).
    if (const auto *BO = P.get<clang::BinaryOperator>()) {
      if (BO->getOpcode() == clang::BO_NE ||
          BO->getOpcode() == clang::BO_EQ) {
        // Check if one side is end().
        for (const auto *Side : {BO->getLHS(), BO->getRHS()}) {
          if (const auto *MC =
                  llvm::dyn_cast<clang::CXXMemberCallExpr>(
                      Side->IgnoreImplicit())) {
            if (const auto *M = MC->getMethodDecl()) {
              if (M->getName() == "end")
                return true;
            }
          }
        }
      }
      return false;
    }
    // Keep walking up through implicit nodes.
    if (P.get<clang::ImplicitCastExpr>() ||
        P.get<clang::MaterializeTemporaryExpr>() ||
        P.get<clang::CXXConstructExpr>() ||
        P.get<clang::UnaryOperator>() ||
        P.get<clang::ExprWithCleanups>()) {
      Parents = Ctx.getParents(P);
      continue;
    }
    break;
  }
  return false;
}

void PreferContainsCheck::registerMatchers(MatchFinder *Finder) {
  auto HasContainsMethod =
      cxxRecordDecl(hasMethod(hasName("contains")));

  // Pattern 1: container.count(key) > 0 or != 0
  Finder->addMatcher(
      binaryOperator(
          hasAnyOperatorName("!=", ">"),
          hasEitherOperand(ignoringImplicit(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(
                      hasName("count"),
                      ofClass(HasContainsMethod))))
                  .bind("count_call"))),
          hasEitherOperand(ignoringImplicit(
              integerLiteral(equals(0)))))
          .bind("count_check"),
      this);

  // Pattern 2: container.find(key) compared with container.end()
  // Instead of trying to match the comparison operator (which may be
  // rewritten in C++20), we match the find() call and walk up in check().
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(
              hasName("find"),
              ofClass(HasContainsMethod))))
          .bind("map_find_call"),
      this);

  // Pattern 3: string.find(x) != std::string::npos
  Finder->addMatcher(
      binaryOperator(
          hasOperatorName("!="),
          hasEitherOperand(
              cxxMemberCallExpr(
                  callee(cxxMethodDecl(
                      hasName("find"),
                      ofClass(cxxRecordDecl(
                          hasMethod(hasName("starts_with")))))))
                  .bind("str_find_call")),
          hasEitherOperand(ignoringImplicit(
              declRefExpr(to(namedDecl(hasName("npos")))))))
          .bind("find_npos_check"),
      this);
}

void PreferContainsCheck::check(
    const MatchFinder::MatchResult &Result) {

  if (Result.Nodes.getNodeAs<clang::BinaryOperator>("count_check")) {
    const auto *Call =
        Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("count_call");
    if (!Call) return;

    diag(Call->getExprLoc(),
         "count() > 0 for existence check is suboptimal; "
         "use .contains() (C++20) which returns bool directly");

    diag(Call->getExprLoc(),
         "on multimap/multiset, count() traverses ALL matching "
         "elements; contains() stops after the first match",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *FindCall =
          Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("map_find_call")) {
    if (isComparedWithEnd(*Result.Context, FindCall)) {
      diag(FindCall->getExprLoc(),
           "find(k) != end() for existence check is verbose; "
           "use .contains(k) (C++20) for clearer intent");

      diag(FindCall->getExprLoc(),
           "contains() returns bool directly, avoiding the unused "
           "iterator and potential dangling-iterator bugs",
           clang::DiagnosticIDs::Note);
    }
    return;
  }

  if (Result.Nodes.getNodeAs<clang::BinaryOperator>("find_npos_check")) {
    const auto *Call =
        Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("str_find_call");
    if (!Call) return;

    diag(Call->getExprLoc(),
         "string.find(x) != npos for substring existence check; "
         "use string.contains(x) (C++23) for clarity");
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
