//===--- PreferFormatCheck.cpp - hl-modernize-prefer-format ---*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferFormatCheck.h"
#include "utils/CppStandardUtils.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferFormatCheck::PreferFormatCheck(llvm::StringRef Name,
                                     clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferFormatCheck::registerMatchers(MatchFinder *Finder) {
  // Match construction of std::stringstream / std::ostringstream.
  Finder->addMatcher(
      varDecl(hasType(namedDecl(
                  matchesName("::std::(basic_)?(o|i)?stringstream"))))
          .bind("sstream_var"),
      this);

  // Match calls to sprintf / snprintf.
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasAnyName("::sprintf", "::snprintf",
                                              "sprintf", "snprintf"))))
          .bind("sprintf_call"),
      this);

  // Match calls to fprintf (for output formatting).
  Finder->addMatcher(
      callExpr(callee(functionDecl(hasAnyName("::fprintf", "fprintf"))))
          .bind("fprintf_call"),
      this);
}

void PreferFormatCheck::check(const MatchFinder::MatchResult &Result) {
  auto Std = utils::detectStandard(*Result.Context);

  if (const auto *SS =
          Result.Nodes.getNodeAs<clang::VarDecl>("sstream_var")) {
    diag(SS->getLocation(),
         "std::stringstream has significant overhead: virtual dispatch, "
         "locale handling, and heap allocation; prefer std::format (C++20)");

    diag(SS->getLocation(),
         "std::format is type-safe, faster, and validates format strings "
         "at compile time",
         clang::DiagnosticIDs::Note);

    if (utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
      diag(SS->getLocation(),
           "for direct output use std::print/std::println (C++23) — "
           "avoids intermediate string allocation entirely",
           clang::DiagnosticIDs::Note);
    }
    return;
  }

  if (const auto *Sp =
          Result.Nodes.getNodeAs<clang::CallExpr>("sprintf_call")) {
    diag(Sp->getExprLoc(),
         "sprintf/snprintf are not type-safe and risk buffer overflow; "
         "prefer std::format (C++20) or std::format_to for "
         "pre-allocated buffers");

    diag(Sp->getExprLoc(),
         "std::format_to writes into an output iterator, allowing "
         "direct output to a pre-sized buffer without intermediate "
         "string allocation",
         clang::DiagnosticIDs::Note);
    return;
  }

  if (const auto *Fp =
          Result.Nodes.getNodeAs<clang::CallExpr>("fprintf_call")) {
    diag(Fp->getExprLoc(),
         "fprintf is not type-safe; prefer std::format (C++20) with "
         "explicit write, or std::print (C++23) for direct output");

    if (utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
      diag(Fp->getExprLoc(),
           "std::print(stream, fmt, args...) provides type-safe "
           "formatted output directly to a stream (C++23)",
           clang::DiagnosticIDs::Note);
    }
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
