//===--- PreferInplaceVectorCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferInplaceVectorCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferInplaceVectorCheck::PreferInplaceVectorCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      MaxInplaceSize(
          Options.get("MaxInplaceSize", 64u)) {}

void PreferInplaceVectorCheck::registerMatchers(MatchFinder *Finder) {
  // Match local std::vector variables with reserve() called with
  // a compile-time constant that is small enough for inplace_vector.
  // This is a heuristic: vectors reserved with small constants are
  // good candidates.
  Finder->addMatcher(
      cxxMemberCallExpr(
          on(declRefExpr(to(
              varDecl(hasType(namedDecl(hasName("::std::vector"))),
                      hasLocalStorage())
                  .bind("vec_var")))),
          callee(cxxMethodDecl(hasName("reserve"))),
          hasArgument(0, integerLiteral().bind("reserve_size")))
          .bind("reserve_call"),
      this);

  // Match local std::vector with constructor taking a size argument.
  Finder->addMatcher(
      varDecl(
          hasType(namedDecl(hasName("::std::vector"))),
          hasLocalStorage(),
          hasInitializer(
              cxxConstructExpr(
                  hasArgument(0, integerLiteral().bind("ctor_size")))
                  .bind("vec_ctor")))
          .bind("sized_vec"),
      this);
}

void PreferInplaceVectorCheck::check(
    const MatchFinder::MatchResult &Result) {
  // Case 1: vector with reserve(N) where N <= MaxInplaceSize.
  if (const auto *ReserveCall =
          Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>(
              "reserve_call")) {
    const auto *SizeLit =
        Result.Nodes.getNodeAs<clang::IntegerLiteral>("reserve_size");
    if (!SizeLit)
      return;

    uint64_t Size = SizeLit->getValue().getZExtValue();
    if (Size > MaxInplaceSize)
      return;

    diag(ReserveCall->getExprLoc(),
         "std::vector with reserve(%0) in local scope; consider "
         "std::inplace_vector<%0> (C++26) to avoid heap allocation")
        << static_cast<unsigned>(Size);

    diag(ReserveCall->getExprLoc(),
         "std::inplace_vector stores all elements inline (on the stack), "
         "eliminating allocator overhead entirely",
         clang::DiagnosticIDs::Note);
    return;
  }

  // Case 2: vector constructed with a size.
  if (const auto *SizedVec =
          Result.Nodes.getNodeAs<clang::VarDecl>("sized_vec")) {
    const auto *SizeLit =
        Result.Nodes.getNodeAs<clang::IntegerLiteral>("ctor_size");
    if (!SizeLit)
      return;

    uint64_t Size = SizeLit->getValue().getZExtValue();
    if (Size > MaxInplaceSize)
      return;

    diag(SizedVec->getLocation(),
         "local std::vector(%0) with small fixed size; consider "
         "std::inplace_vector<T, %0> (C++26) for zero-allocation storage")
        << static_cast<unsigned>(Size);

    diag(SizedVec->getLocation(),
         "for pre-C++26 code, boost::static_vector or "
         "absl::InlinedVector provide similar stack-based storage",
         clang::DiagnosticIDs::Note);
    return;
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
