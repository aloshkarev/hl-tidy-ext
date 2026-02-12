//===--- PreferVectorOverListCheck.cpp - hl-perf-prefer-vector --*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferVectorOverListCheck.h"
#include "utils/CppStandardUtils.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferVectorOverListCheck::PreferVectorOverListCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferVectorOverListCheck::registerMatchers(MatchFinder *Finder) {
  // std::list
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::list")))).bind("list_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::list")))).bind("list_field"),
      this);

  // std::forward_list
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::forward_list"))))
          .bind("fwd_list_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::forward_list"))))
          .bind("fwd_list_field"),
      this);
}

void PreferVectorOverListCheck::check(
    const MatchFinder::MatchResult &Result) {
  auto Std = utils::detectStandard(*Result.Context);
  clang::SourceLocation Loc;
  llvm::StringRef Container;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("list_var")) {
    Loc = V->getLocation();
    Container = "std::list";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("list_field")) {
    Loc = F->getLocation();
    Container = "std::list";
  } else if (const auto *V =
                 Result.Nodes.getNodeAs<clang::VarDecl>("fwd_list_var")) {
    Loc = V->getLocation();
    Container = "std::forward_list";
  } else if (const auto *F = Result.Nodes.getNodeAs<clang::FieldDecl>(
                 "fwd_list_field")) {
    Loc = F->getLocation();
    Container = "std::forward_list";
  } else {
    return;
  }

  diag(Loc,
       "%0 has poor cache locality due to node-based allocation; "
       "prefer std::vector for contiguous memory and cache-friendly iteration")
      << Container;

  diag(Loc,
       "std::vector is faster for iteration, push_back, and even random "
       "insertion for up to ~10K elements due to CPU cache effects",
       clang::DiagnosticIDs::Note);

  if (utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
    diag(Loc,
         "if you need stable iterators/pointers, consider "
         "boost::stable_vector or a pool allocator with std::vector",
         clang::DiagnosticIDs::Note);
  }

  if (utils::hasAtLeast(Std, utils::CppStandard::Cpp26)) {
    diag(Loc,
         "std::hive (C++26) provides stable pointers with better "
         "cache locality than linked lists",
         clang::DiagnosticIDs::Note);
  }
}

} // namespace checks
} // namespace tidy
} // namespace hl
