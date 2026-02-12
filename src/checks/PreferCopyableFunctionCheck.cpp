//===--- PreferCopyableFunctionCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferCopyableFunctionCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferCopyableFunctionCheck::PreferCopyableFunctionCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferCopyableFunctionCheck::registerMatchers(MatchFinder *Finder) {
  // Match std::function variable declarations.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("func_var"),
      this);

  // Match std::function field declarations.
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("func_field"),
      this);

  // Match std::function parameter declarations.
  Finder->addMatcher(
      parmVarDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("func_param"),
      this);
}

void PreferCopyableFunctionCheck::check(
    const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;
  llvm::StringRef Kind;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("func_var")) {
    Loc = V->getLocation();
    Kind = "variable";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("func_field")) {
    Loc = F->getLocation();
    Kind = "field";
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>("func_param")) {
    Loc = P->getLocation();
    Kind = "parameter";
  } else {
    return;
  }

  diag(Loc,
       "std::function %0 can be replaced with std::copyable_function "
       "(C++26) for stricter const/noexcept correctness")
      << Kind;

  diag(Loc,
       "std::copyable_function enforces that operator() const only "
       "invokes const-qualified callables, preventing subtle bugs "
       "with mutable lambdas",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "if the callable does not need to be copied, prefer "
       "std::move_only_function (C++23) or std::function_ref (C++26) "
       "for even lower overhead",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
