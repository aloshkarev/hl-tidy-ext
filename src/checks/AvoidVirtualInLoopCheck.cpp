//===--- AvoidVirtualInLoopCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidVirtualInLoopCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidVirtualInLoopCheck::AvoidVirtualInLoopCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidVirtualInLoopCheck::registerMatchers(MatchFinder *Finder) {
  // Match virtual method calls inside any loop construct.
  auto VirtualCallMatcher =
      cxxMemberCallExpr(
          on(hasType(pointsTo(cxxRecordDecl()))),
          callee(cxxMethodDecl(isVirtual())))
          .bind("vcall_in_loop");

  // Also match calls through references.
  auto VirtualCallRefMatcher =
      cxxMemberCallExpr(
          on(hasType(references(cxxRecordDecl()))),
          callee(cxxMethodDecl(isVirtual())))
          .bind("vcall_in_loop");

  // For loops.
  Finder->addMatcher(
      forStmt(hasDescendant(VirtualCallMatcher)), this);
  Finder->addMatcher(
      forStmt(hasDescendant(VirtualCallRefMatcher)), this);

  // While loops.
  Finder->addMatcher(
      whileStmt(hasDescendant(VirtualCallMatcher)), this);
  Finder->addMatcher(
      whileStmt(hasDescendant(VirtualCallRefMatcher)), this);

  // Do-while loops.
  Finder->addMatcher(
      doStmt(hasDescendant(VirtualCallMatcher)), this);
  Finder->addMatcher(
      doStmt(hasDescendant(VirtualCallRefMatcher)), this);

  // Range-for loops.
  Finder->addMatcher(
      cxxForRangeStmt(hasDescendant(VirtualCallMatcher)), this);
  Finder->addMatcher(
      cxxForRangeStmt(hasDescendant(VirtualCallRefMatcher)), this);
}

void AvoidVirtualInLoopCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *VCall =
      Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("vcall_in_loop");
  if (!VCall)
    return;

  const auto *Method = VCall->getMethodDecl();
  if (!Method)
    return;

  diag(VCall->getExprLoc(),
       "virtual call to '%0' inside a loop: indirect dispatch prevents "
       "inlining and causes branch predictor misses")
      << Method->getNameAsString();

  diag(VCall->getExprLoc(),
       "consider CRTP, std::variant + std::visit, or 'if constexpr' "
       "with type tags for compile-time dispatch",
       clang::DiagnosticIDs::Note);

  diag(VCall->getExprLoc(),
       "if dynamic dispatch is required, cache the function pointer "
       "before the loop or batch-process by concrete type",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
