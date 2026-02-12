//===--- PreferEraseIfCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferEraseIfCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferEraseIfCheck::PreferEraseIfCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferEraseIfCheck::registerMatchers(MatchFinder *Finder) {
  // Match the erase-remove idiom:
  //   container.erase(std::remove_if(...), container.end())
  // We detect: a call to member .erase() where the first argument is
  // a call to std::remove or std::remove_if.
  // The first argument to .erase() is typically wrapped in several
  // layers of ImplicitCastExpr + CXXConstructExpr for iterator
  // conversion (iterator -> const_iterator).  Use hasDescendant to
  // look through all wrapping nodes.
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(hasName("erase"))),
          argumentCountIs(2),
          hasDescendant(
              callExpr(callee(functionDecl(
                  hasAnyName("remove", "remove_if"))))
                  .bind("remove_call")))
          .bind("erase_call"),
      this);
}

void PreferEraseIfCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Erase =
      Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("erase_call");
  const auto *Remove =
      Result.Nodes.getNodeAs<clang::CallExpr>("remove_call");
  if (!Erase || !Remove)
    return;

  // Determine which std::remove variant was used.
  const auto *RemoveDecl = Remove->getDirectCallee();
  bool IsRemoveIf = RemoveDecl && RemoveDecl->getName() == "remove_if";

  if (IsRemoveIf) {
    diag(Erase->getExprLoc(),
         "erase-remove_if idiom is verbose and error-prone; "
         "use std::erase_if(container, predicate) (C++20)");
  } else {
    diag(Erase->getExprLoc(),
         "erase-remove idiom is verbose and error-prone; "
         "use std::erase(container, value) (C++20)");
  }

  diag(Erase->getExprLoc(),
       "std::erase_if works uniformly on all standard containers "
       "(vector, deque, string, list, set, map, etc.) and returns "
       "the count of erased elements",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
