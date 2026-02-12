//===--- PreferToUnderlyingCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferToUnderlyingCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferToUnderlyingCheck::PreferToUnderlyingCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferToUnderlyingCheck::registerMatchers(MatchFinder *Finder) {
  // Match static_cast<IntegerType>(expr) where expr has scoped enum type.
  Finder->addMatcher(
      cxxStaticCastExpr(
          hasDestinationType(qualType(
              hasCanonicalType(isInteger()))),
          hasSourceExpression(
              hasType(qualType(hasCanonicalType(
                  hasDeclaration(enumDecl(isScoped()).bind("enum_decl")))))))
          .bind("enum_cast"),
      this);
}

void PreferToUnderlyingCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Cast =
      Result.Nodes.getNodeAs<clang::CXXStaticCastExpr>("enum_cast");
  const auto *Enum =
      Result.Nodes.getNodeAs<clang::EnumDecl>("enum_decl");
  if (!Cast || !Enum)
    return;

  diag(Cast->getExprLoc(),
       "static_cast of scoped enum '%0' to integer; "
       "use std::to_underlying() (C++23) for type-safe conversion")
      << Enum->getNameAsString();

  diag(Cast->getExprLoc(),
       "std::to_underlying() always returns the correct underlying "
       "type, preventing silent truncation or widening bugs",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
