//===--- PreferNoexceptMoveCheck.cpp - hl-perf-prefer-noexcept-move *- C++ *-//
// Author: Aleksandr Loshkarev

#include "PreferNoexceptMoveCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferNoexceptMoveCheck::PreferNoexceptMoveCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferNoexceptMoveCheck::registerMatchers(MatchFinder *Finder) {
  // Match move constructors without noexcept.
  Finder->addMatcher(
      cxxConstructorDecl(
          isMoveConstructor(),
          isDefinition(),
          unless(isDefaulted()),
          unless(isDeleted()),
          unless(isNoThrow()))
          .bind("move_ctor"),
      this);

  // Match move assignment operators without noexcept.
  Finder->addMatcher(
      cxxMethodDecl(
          isMoveAssignmentOperator(),
          isDefinition(),
          unless(isDefaulted()),
          unless(isDeleted()),
          unless(isNoThrow()))
          .bind("move_assign"),
      this);
}

void PreferNoexceptMoveCheck::check(
    const MatchFinder::MatchResult &Result) {
  if (const auto *Ctor =
          Result.Nodes.getNodeAs<clang::CXXConstructorDecl>("move_ctor")) {
    diag(Ctor->getLocation(),
         "move constructor is not noexcept; std::vector will COPY "
         "instead of MOVE elements during reallocation");

    diag(Ctor->getLocation(),
         "add noexcept to enable move-based reallocation in "
         "std::vector, std::swap, and standard algorithms",
         clang::DiagnosticIDs::Note);

    diag(Ctor->getLocation(),
         "a missing noexcept turns O(N) reallocation into "
         "O(N * copy_cost) — one of the most impactful C++ "
         "performance issues",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Assign =
          Result.Nodes.getNodeAs<clang::CXXMethodDecl>("move_assign")) {
    diag(Assign->getLocation(),
         "move assignment operator is not noexcept; this disables "
         "move optimizations in standard containers and algorithms");

    diag(Assign->getLocation(),
         "add noexcept to enable efficient move operations; "
         "CppCoreGuidelines C.66 requires noexcept move operations",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
