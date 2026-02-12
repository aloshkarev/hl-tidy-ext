//===--- AvoidCoutCerrCheck.cpp - hl-perf-avoid-cout-cerr -----*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidCoutCerrCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidCoutCerrCheck::AvoidCoutCerrCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidCoutCerrCheck::registerMatchers(MatchFinder *Finder) {
  // Match references to std::cout.
  Finder->addMatcher(
      declRefExpr(to(namedDecl(hasName("::std::cout")))).bind("cout_ref"),
      this);

  // Match references to std::cerr.
  Finder->addMatcher(
      declRefExpr(to(namedDecl(hasName("::std::cerr")))).bind("cerr_ref"),
      this);

  // Match references to std::clog.
  Finder->addMatcher(
      declRefExpr(to(namedDecl(hasName("::std::clog")))).bind("clog_ref"),
      this);
}

void AvoidCoutCerrCheck::check(const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;
  llvm::StringRef Stream;

  if (const auto *R =
          Result.Nodes.getNodeAs<clang::DeclRefExpr>("cout_ref")) {
    Loc = R->getExprLoc();
    Stream = "std::cout";
  } else if (const auto *R =
                 Result.Nodes.getNodeAs<clang::DeclRefExpr>("cerr_ref")) {
    Loc = R->getExprLoc();
    Stream = "std::cerr";
  } else if (const auto *R =
                 Result.Nodes.getNodeAs<clang::DeclRefExpr>("clog_ref")) {
    Loc = R->getExprLoc();
    Stream = "std::clog";
  } else {
    return;
  }

  diag(Loc,
       "%0 uses a global mutex (sync_with_stdio) causing contention "
       "in multi-threaded code; avoid in production services")
      << Stream;

  diag(Loc,
       "use a logging framework (spdlog, glog, or custom) that supports "
       "async logging, log levels, and structured output",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "for non-logging output, consider fmt::print or std::format "
       "(C++20) with explicit file descriptor writes",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
