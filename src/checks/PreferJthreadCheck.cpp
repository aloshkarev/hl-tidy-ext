//===--- PreferJthreadCheck.cpp - hl-modernize-prefer-jthread --*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferJthreadCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferJthreadCheck::PreferJthreadCheck(llvm::StringRef Name,
                                       clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferJthreadCheck::registerMatchers(MatchFinder *Finder) {
  // Match variable declarations of std::thread.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::thread")))).bind("thread_var"),
      this);

  // Match field declarations of std::thread.
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::thread"))))
          .bind("thread_field"),
      this);
}

void PreferJthreadCheck::check(const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("thread_var")) {
    Loc = V->getLocation();
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("thread_field")) {
    Loc = F->getLocation();
  } else {
    return;
  }

  diag(Loc,
       "std::thread requires manual join/detach; prefer std::jthread "
       "(C++20) which auto-joins on destruction and supports "
       "cooperative cancellation via std::stop_token");

  diag(Loc,
       "std::jthread eliminates common bugs: dangling threads, "
       "std::terminate on non-joined threads, and manual cancellation logic",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "for high-load services, std::jthread with std::stop_token "
       "enables graceful shutdown without external synchronization",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
