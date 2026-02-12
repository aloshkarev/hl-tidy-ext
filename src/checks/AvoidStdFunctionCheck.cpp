//===--- AvoidStdFunctionCheck.cpp - hl-perf-avoid-std-function --*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidStdFunctionCheck.h"
#include "utils/CppStandardUtils.h"
#include "utils/DiagnosticHelper.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidStdFunctionCheck::AvoidStdFunctionCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidStdFunctionCheck::registerMatchers(MatchFinder *Finder) {
  // Match variable declarations whose type is std::function<...>.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("var"),
      this);

  // Match function parameters typed as std::function<...>.
  Finder->addMatcher(
      parmVarDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("param"),
      this);

  // Match field declarations (class members).
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::function"))))
          .bind("field"),
      this);

  // Match typedef / using aliases of std::function.
  // Use hasUnderlyingType to match the aliased type.
  Finder->addMatcher(
      typeAliasDecl(hasUnderlyingType(
          hasDeclaration(namedDecl(hasName("::std::function")))))
          .bind("alias"),
      this);
}

void AvoidStdFunctionCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Ctx = Result.Context;
  auto Std = utils::detectStandard(*Ctx);

  clang::SourceLocation Loc;
  llvm::StringRef Kind;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("var")) {
    Loc = V->getLocation();
    Kind = "variable";
  } else if (const auto *P =
                 Result.Nodes.getNodeAs<clang::ParmVarDecl>("param")) {
    Loc = P->getLocation();
    Kind = "parameter";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("field")) {
    Loc = F->getLocation();
    Kind = "field";
  } else if (const auto *A =
                 Result.Nodes.getNodeAs<clang::TypeAliasDecl>("alias")) {
    Loc = A->getLocation();
    Kind = "type alias";
  } else {
    return;
  }

  // Primary warning.
  diag(Loc,
       "std::function causes heap allocation and type-erasure overhead; "
       "avoid in high-load / hot-path code (%0)")
      << Kind;

  // Standard-aware replacement notes.
  diag(Loc,
       "for non-owning callable references use a template parameter or "
       "a function pointer",
       clang::DiagnosticIDs::Note);

  if (utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
    diag(Loc,
         "if ownership transfer is needed, consider "
         "std::move_only_function (C++23) — avoids copy overhead",
         clang::DiagnosticIDs::Note);
  }

  if (utils::hasAtLeast(Std, utils::CppStandard::Cpp26)) {
    diag(Loc,
         "for non-owning callable parameters, consider "
         "std::function_ref (C++26) — zero-overhead type-erased reference",
         clang::DiagnosticIDs::Note);
  }

  // If the project is limited to C++17/20, mention future alternatives.
  if (!utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
    diag(Loc,
         "std::move_only_function (C++23) and std::function_ref (C++26) "
         "are better alternatives; upgrade standard when feasible",
         clang::DiagnosticIDs::Note);
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
