//===--- AvoidStdRegexCheck.cpp - hl-perf-avoid-std-regex -------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "AvoidStdRegexCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

AvoidStdRegexCheck::AvoidStdRegexCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void AvoidStdRegexCheck::registerMatchers(MatchFinder *Finder) {
  // Match construction of std::regex / std::basic_regex.
  Finder->addMatcher(
      cxxConstructExpr(hasType(namedDecl(matchesName("::std::(basic_)?regex"))))
          .bind("regex_ctor"),
      this);

  // Match variable declarations of regex type.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(matchesName("::std::(basic_)?regex"))))
          .bind("regex_var"),
      this);

  // Match calls to std::regex_search, std::regex_match, std::regex_replace.
  Finder->addMatcher(
      callExpr(callee(functionDecl(
                   matchesName("::std::regex_(search|match|replace)"))))
          .bind("regex_call"),
      this);
}

void AvoidStdRegexCheck::check(const MatchFinder::MatchResult &Result) {
  clang::SourceLocation Loc;

  if (const auto *Ctor =
          Result.Nodes.getNodeAs<clang::CXXConstructExpr>("regex_ctor")) {
    Loc = Ctor->getExprLoc();
  } else if (const auto *Var =
                 Result.Nodes.getNodeAs<clang::VarDecl>("regex_var")) {
    Loc = Var->getLocation();
  } else if (const auto *Call =
                 Result.Nodes.getNodeAs<clang::CallExpr>("regex_call")) {
    Loc = Call->getExprLoc();
  } else {
    return;
  }

  diag(Loc,
       "std::regex has extremely poor performance: dynamic pattern "
       "compilation, heavy heap allocations, and no compile-time "
       "optimizations; banned in Chromium and other high-load projects");

  diag(Loc,
       "use CTRE (compile-time regular expressions) for static patterns, "
       "or Google RE2 for runtime patterns with linear-time guarantees",
       clang::DiagnosticIDs::Note);

  diag(Loc,
       "for simple string matching consider std::string::find(), "
       "std::string_view::find(), or hand-written parsers",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
