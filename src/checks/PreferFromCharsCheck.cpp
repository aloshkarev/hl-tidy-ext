//===--- PreferFromCharsCheck.cpp - hl-perf-prefer-from-chars ---*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferFromCharsCheck.h"
#include "utils/CppStandardUtils.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferFromCharsCheck::PreferFromCharsCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferFromCharsCheck::registerMatchers(MatchFinder *Finder) {
  // Match calls to std::stoi, std::stol, std::stoll, std::stoul,
  // std::stoull, std::stof, std::stod, std::stold.
  Finder->addMatcher(
      callExpr(callee(functionDecl(matchesName(
                   "::std::sto(i|l|ll|ul|ull|f|d|ld)"))))
          .bind("sto_call"),
      this);

  // Match calls to std::to_string.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasName("::std::to_string"))))
          .bind("to_string_call"),
      this);

  // Match atoi, atol, atof from C.
  Finder->addMatcher(
      callExpr(callee(functionDecl(matchesName("^::ato(i|l|f|ll)$"))))
          .bind("ato_call"),
      this);
}

void PreferFromCharsCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *StoCall =
          Result.Nodes.getNodeAs<clang::CallExpr>("sto_call")) {
    diag(StoCall->getExprLoc(),
         "std::sto* functions throw exceptions on invalid input and "
         "use locale-dependent parsing (global locale lock contention); "
         "prefer std::from_chars (C++17)");

    diag(StoCall->getExprLoc(),
         "std::from_chars is locale-independent, exception-free, and "
         "performs no heap allocations — up to 20x faster",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *TsCall =
          Result.Nodes.getNodeAs<clang::CallExpr>("to_string_call")) {
    diag(TsCall->getExprLoc(),
         "std::to_string allocates heap memory and depends on the global "
         "locale; prefer std::to_chars (C++17) for high-throughput code");

    diag(TsCall->getExprLoc(),
         "std::to_chars writes directly into a caller-provided buffer, "
         "avoiding allocation entirely",
         clang::DiagnosticIDs::Note);

    auto Std = utils::detectStandard(*Result.Context);
    if (utils::hasAtLeast(Std, utils::CppStandard::Cpp20)) {
      diag(TsCall->getExprLoc(),
           "alternatively, std::format (C++20) provides type-safe "
           "formatting with better performance than std::to_string",
           clang::DiagnosticIDs::Note);
    }
    return;
  }

  if (const auto *AtoCall =
          Result.Nodes.getNodeAs<clang::CallExpr>("ato_call")) {
    diag(AtoCall->getExprLoc(),
         "C-style ato* functions have undefined behavior on overflow "
         "and no error reporting; prefer std::from_chars (C++17)");

    diag(AtoCall->getExprLoc(),
         "std::from_chars reports errors via std::errc and handles "
         "overflow safely",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
