//===--- PreferSpanCheck.cpp - hl-modernize-prefer-span -------*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferSpanCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferSpanCheck::PreferSpanCheck(llvm::StringRef Name,
                                 clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferSpanCheck::registerMatchers(MatchFinder *Finder) {
  // Match function parameters that are 'const std::vector<T>&' —
  // these can often be replaced with std::span<const T>.
  Finder->addMatcher(
      parmVarDecl(
          hasType(references(qualType(
              isConstQualified(),
              hasDeclaration(namedDecl(hasName("::std::vector")))))))
          .bind("vec_ref_param"),
      this);

  // Match function declarations that have consecutive (T*, size_t) params.
  // This is a simplified heuristic — we match any function parameter
  // that is a pointer type followed by an integer type.
  Finder->addMatcher(
      functionDecl(
          forEachDescendant(
              parmVarDecl(hasType(pointerType())).bind("ptr_param")))
          .bind("func_with_ptr"),
      this);
}

void PreferSpanCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *VecParam =
          Result.Nodes.getNodeAs<clang::ParmVarDecl>("vec_ref_param")) {
    diag(VecParam->getLocation(),
         "'const std::vector<T>&' parameter ties the interface to "
         "std::vector; prefer std::span<const T> (C++20) to accept "
         "any contiguous range (vector, array, C-array)");

    diag(VecParam->getLocation(),
         "std::span is a non-owning view with zero overhead; "
         "it decouples the interface from the container type",
         clang::DiagnosticIDs::Note);
    return;
  }

  // For pointer parameters, provide a lighter suggestion.
  if (const auto *PtrParam =
          Result.Nodes.getNodeAs<clang::ParmVarDecl>("ptr_param")) {
    // Only flag if parameter name suggests it's a buffer/data pointer.
    auto Name = PtrParam->getName();
    if (Name.empty())
      return;

    // Heuristic: flag params with suggestive names.
    bool IsSuggestive =
        Name.contains_insensitive("buf") ||
        Name.contains_insensitive("data") ||
        Name.contains_insensitive("ptr") ||
        Name.contains_insensitive("arr") ||
        Name.contains_insensitive("items") ||
        Name.contains_insensitive("elems");

    if (!IsSuggestive)
      return;

    diag(PtrParam->getLocation(),
         "raw pointer parameter '%0' with suggestive name; "
         "consider std::span (C++20) for bounds-safe contiguous access")
        << Name;

    diag(PtrParam->getLocation(),
         "std::span encapsulates pointer+size, preventing buffer "
         "overruns and making the API self-documenting",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
