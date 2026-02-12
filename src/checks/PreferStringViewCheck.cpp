//===--- PreferStringViewCheck.cpp - hl-perf-prefer-string-view -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferStringViewCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferStringViewCheck::PreferStringViewCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferStringViewCheck::registerMatchers(MatchFinder *Finder) {
  // Match function parameters of type 'const std::string &'.
  // We look for ParmVarDecl where the type is a reference to a const
  // std::string (or std::basic_string<char>).
  Finder->addMatcher(
      parmVarDecl(
          hasType(references(qualType(
              isConstQualified(),
              hasDeclaration(namedDecl(hasName("::std::basic_string")))))))
          .bind("param"),
      this);
}

void PreferStringViewCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Param =
      Result.Nodes.getNodeAs<clang::ParmVarDecl>("param");
  if (!Param)
    return;

  // Skip if the function body modifies the string (takes address, etc.)
  // This is a heuristic — we flag all const string& params.
  diag(Param->getLocation(),
       "'const std::string&' parameter causes potential heap allocation "
       "when called with string literals or substrings; "
       "prefer std::string_view for read-only access");

  diag(Param->getLocation(),
       "std::string_view is a non-owning view that avoids copies; "
       "available since C++17",
       clang::DiagnosticIDs::Note);

  diag(Param->getLocation(),
       "note: if the function stores the string, keep std::string "
       "parameter (by value with move) for ownership transfer",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
