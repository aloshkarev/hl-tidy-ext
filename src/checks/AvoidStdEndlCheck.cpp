//===--- AvoidStdEndlCheck.cpp - hl-perf-avoid-std-endl --------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidStdEndlCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidStdEndlCheck::AvoidStdEndlCheck(llvm::StringRef Name,
                                     clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidStdEndlCheck::registerMatchers(MatchFinder *Finder) {
  // Match any reference to std::endl.
  Finder->addMatcher(
      declRefExpr(to(namedDecl(hasName("::std::endl")))).bind("endl"),
      this);
}

void AvoidStdEndlCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *EndlRef =
      Result.Nodes.getNodeAs<clang::DeclRefExpr>("endl");
  if (!EndlRef)
    return;

  auto Loc = EndlRef->getExprLoc();

  // Calculate the source range for the FixIt replacement.
  auto &SM = *Result.SourceManager;
  auto &LO = Result.Context->getLangOpts();
  auto Range = clang::CharSourceRange::getTokenRange(
      EndlRef->getSourceRange());
  auto EndlText = clang::Lexer::getSourceText(Range, SM, LO);

  auto D = diag(Loc,
                "std::endl forces a stream flush on every call; "
                "use '\\n' instead — up to 10x faster in I/O-intensive code");

  // Provide a FixIt hint: replace std::endl → '\n'.
  if (!EndlText.empty()) {
    D << clang::FixItHint::CreateReplacement(Range, "'\\n'");
  }

  diag(Loc,
       "if you need an explicit flush, call stream.flush() separately "
       "to make the intent clear",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
