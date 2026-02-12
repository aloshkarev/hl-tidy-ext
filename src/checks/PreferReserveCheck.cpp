//===--- PreferReserveCheck.cpp - hl-perf-prefer-reserve -------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferReserveCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferReserveCheck::PreferReserveCheck(llvm::StringRef Name,
                                       clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferReserveCheck::registerMatchers(MatchFinder *Finder) {
  // Match push_back/emplace_back calls on vectors inside any loop construct.
  // forStmt, whileStmt, doStmt, cxxForRangeStmt
  auto PushBackMatcher =
      cxxMemberCallExpr(
          on(hasType(namedDecl(hasName("::std::vector")))),
          callee(cxxMethodDecl(hasAnyName("push_back", "emplace_back"))))
          .bind("push_in_loop");

  Finder->addMatcher(
      forStmt(hasDescendant(PushBackMatcher)).bind("for_loop"), this);

  Finder->addMatcher(
      whileStmt(hasDescendant(PushBackMatcher)).bind("while_loop"), this);

  Finder->addMatcher(
      doStmt(hasDescendant(PushBackMatcher)).bind("do_loop"), this);

  Finder->addMatcher(
      cxxForRangeStmt(hasDescendant(PushBackMatcher)).bind("range_loop"),
      this);
}

void PreferReserveCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Push =
      Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("push_in_loop");
  if (!Push)
    return;

  diag(Push->getExprLoc(),
       "push_back/emplace_back inside a loop without reserve() causes "
       "repeated heap reallocations; call reserve() before the loop "
       "if the size is known or estimable");

  diag(Push->getExprLoc(),
       "each reallocation copies/moves all existing elements and "
       "invalidates iterators; pre-allocating avoids O(N log N) total copies",
       clang::DiagnosticIDs::Note);

  diag(Push->getExprLoc(),
       "if the final size is unknown, consider reserving an estimated "
       "upper bound, or use std::vector::resize() + index assignment",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
