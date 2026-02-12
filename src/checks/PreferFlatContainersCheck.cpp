//===--- PreferFlatContainersCheck.cpp -*- C++ -*-===//
// Author: Aleksandr Loshkarev

#include "PreferFlatContainersCheck.h"
#include "utils/CppStandardUtils.h"

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace hl {
namespace tidy {
namespace checks {

PreferFlatContainersCheck::PreferFlatContainersCheck(
    llvm::StringRef Name, clang::tidy::ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void PreferFlatContainersCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::map")))).bind("map_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::map")))).bind("map_field"),
      this);
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::set")))).bind("set_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::set")))).bind("set_field"),
      this);
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::multimap"))))
          .bind("mmap_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::multimap"))))
          .bind("mmap_field"),
      this);
  Finder->addMatcher(
      varDecl(hasType(namedDecl(hasName("::std::multiset"))))
          .bind("mset_var"),
      this);
  Finder->addMatcher(
      fieldDecl(hasType(namedDecl(hasName("::std::multiset"))))
          .bind("mset_field"),
      this);
}

void PreferFlatContainersCheck::check(
    const MatchFinder::MatchResult &Result) {
  auto Std = utils::detectStandard(*Result.Context);

  clang::SourceLocation Loc;
  llvm::StringRef Container;
  llvm::StringRef FlatAlt;

  if (const auto *V = Result.Nodes.getNodeAs<clang::VarDecl>("map_var")) {
    Loc = V->getLocation();
    Container = "std::map";
    FlatAlt = "std::flat_map";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("map_field")) {
    Loc = F->getLocation();
    Container = "std::map";
    FlatAlt = "std::flat_map";
  } else if (const auto *V =
                 Result.Nodes.getNodeAs<clang::VarDecl>("set_var")) {
    Loc = V->getLocation();
    Container = "std::set";
    FlatAlt = "std::flat_set";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("set_field")) {
    Loc = F->getLocation();
    Container = "std::set";
    FlatAlt = "std::flat_set";
  } else if (const auto *V =
                 Result.Nodes.getNodeAs<clang::VarDecl>("mmap_var")) {
    Loc = V->getLocation();
    Container = "std::multimap";
    FlatAlt = "std::flat_multimap";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("mmap_field")) {
    Loc = F->getLocation();
    Container = "std::multimap";
    FlatAlt = "std::flat_multimap";
  } else if (const auto *V =
                 Result.Nodes.getNodeAs<clang::VarDecl>("mset_var")) {
    Loc = V->getLocation();
    Container = "std::multiset";
    FlatAlt = "std::flat_multiset";
  } else if (const auto *F =
                 Result.Nodes.getNodeAs<clang::FieldDecl>("mset_field")) {
    Loc = F->getLocation();
    Container = "std::multiset";
    FlatAlt = "std::flat_multiset";
  } else {
    return;
  }

  diag(Loc,
       "%0 is a tree-based container with per-node heap allocation "
       "and poor cache locality; consider a flat/contiguous alternative")
      << Container;

  if (utils::hasAtLeast(Std, utils::CppStandard::Cpp23)) {
    diag(Loc,
         "use %0 (C++23): contiguous storage with cache-friendly "
         "lookup and iteration",
         clang::DiagnosticIDs::Note)
        << FlatAlt;
  } else {
    diag(Loc,
         "use a sorted std::vector with std::lower_bound, or "
         "boost::container::flat_map/flat_set as a drop-in replacement",
         clang::DiagnosticIDs::Note);
    diag(Loc,
         "%0 will be available in C++23; upgrade standard when feasible",
         clang::DiagnosticIDs::Note)
        << FlatAlt;
  }

  diag(Loc,
       "if O(1) average lookup is needed, consider "
       "std::unordered_map/unordered_set or absl::flat_hash_map",
       clang::DiagnosticIDs::Note);
}

} // namespace checks
} // namespace tidy
} // namespace hl
