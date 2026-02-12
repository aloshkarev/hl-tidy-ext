//===--- AvoidStdAnyCheck.cpp - hl-perf-avoid-std-any ---------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidStdAnyCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidStdAnyCheck::AvoidStdAnyCheck(llvm::StringRef Name,
                                   clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidStdAnyCheck::registerMatchers(MatchFinder *Finder) {
  // Variable declarations with std::any type.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::any")))).bind("any_var"),
      this);

  // Field declarations.
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::any")))).bind("any_field"),
      this);

  // Function parameters.
  Finder->addMatcher(
      parmVarDecl(hasType(namedDecl(hasName("::std::any")))).bind("any_param"),
      this);

  // Calls to std::any_cast (indicates std::any usage).
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::any_cast"))))
          .bind("any_cast"),
      this);
}

void AvoidStdAnyCheck::check(const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("any_var")) {
    Loc = V->getLocation();
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("any_field")) {
    Loc = F->getLocation();
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>("any_param")) {
    Loc = P->getLocation();
  } else if (const auto *C =
                 Result.Nodes.getNodeAs<clang::CallExpr>("any_cast")) {
    Loc = C->getExprLoc();
  } else {
    return;
  }

  diag(Loc,
       "std::any performs heap allocation for non-trivial types and "
       "requires RTTI; prefer std::variant for a closed set of types");

  diag(Loc,
       "std::variant is stored inline (no heap allocation), type-safe, "
       "and allows exhaustive visitation via std::visit",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "if the type set is truly open, consider template-based "
       "polymorphism or a custom type-erased wrapper with SBO",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
