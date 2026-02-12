//===--- AvoidDynamicCastCheck.cpp - hl-perf-avoid-dynamic-cast -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidDynamicCastCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidDynamicCastCheck::AvoidDynamicCastCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidDynamicCastCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxDynamicCastExpr().bind("dyn_cast"), this);
}

void AvoidDynamicCastCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Cast =
      Result.Nodes.getNodeAs<clang::CXXDynamicCastExpr>("dyn_cast");
  if (!Cast)
    return;

  diag(Cast->getExprLoc(),
       "dynamic_cast requires RTTI and performs O(depth) vtable "
       "traversal at runtime; avoid in performance-critical code");

  diag(Cast->getExprLoc(),
       "use static_cast with a type-tag enum field for a zero-overhead "
       "alternative, or use the visitor pattern / std::variant for "
       "closed type hierarchies",
       clang::DiagnosticIDs::Note);

  diag(Cast->getExprLoc(),
       "Chromium, LLVM, and most game engines build with -fno-rtti; "
       "consider CRTP for compile-time polymorphism",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
