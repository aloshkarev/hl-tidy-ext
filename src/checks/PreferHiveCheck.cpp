//===--- PreferHiveCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferHiveCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferHiveCheck::PreferHiveCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferHiveCheck::registerMatchers(MatchFinder *Finder) {
  // Match std::list variable and field declarations.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::list"))))
          .bind("list_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::list"))))
          .bind("list_field"),
      this);

  // Match std::forward_list variable and field declarations.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::forward_list"))))
          .bind("fwd_list_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::forward_list"))))
          .bind("fwd_list_field"),
      this);
}

void PreferHiveCheck::check(
    const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;
  llvm::StringRef Container;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("list_var")) {
    Loc = V->getLocation();
    Container = "std::list";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("list_field")) {
    Loc = F->getLocation();
    Container = "std::list";
  } else if (const auto *V =
                 Result.Nodes.getNodeAs<clang::VarDecl>("fwd_list_var")) {
    Loc = V->getLocation();
    Container = "std::forward_list";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("fwd_list_field")) {
    Loc = F->getLocation();
    Container = "std::forward_list";
  } else {
    return;
  }

  diag(Loc,
       "%0 has per-node allocation and cache-hostile traversal; "
       "consider std::hive (C++26) for stable pointers with "
       "cache-friendly iteration")
      << Container;

  diag(Loc,
       "std::hive stores elements in contiguous blocks with a "
       "skipfield; provides O(1) insert/erase, stable pointers, "
       "and 5-20x faster iteration than linked lists",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "if pointer/iterator stability is not needed, prefer "
       "std::vector which has the best cache locality; "
       "for pre-C++26, consider plf::colony",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
