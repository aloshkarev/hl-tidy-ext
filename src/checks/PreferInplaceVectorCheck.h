//===--- PreferInplaceVectorCheck.h -*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Flags small fixed-capacity std::vector usage patterns and recommends
// std::inplace_vector (C++26) which stores elements inline without
// any heap allocation.
//
// Also detects std::array used with a size counter (manual dynamic array
// on stack) and suggests std::inplace_vector as a safer alternative.
//
// std::inplace_vector<T, N> advantages:
//   - Zero heap allocations (all storage is inline/on stack)
//   - Same API as std::vector (push_back, size, iterators, etc.)
//   - Ideal for small, bounded collections in hot paths
//   - Replaces boost::static_vector / absl::InlinedVector
//
// References:
//   - P0843R14 (std::inplace_vector)
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_CHECKS_PREFER_INPLACE_VECTOR_CHECK_H
#define HL_TIDY_CHECKS_PREFER_INPLACE_VECTOR_CHECK_H

#include "clang-tidy/ClangTidyCheck.h"

namespace hl {
namespace tidy {
namespace checks {

class PreferInplaceVectorCheck : public clang::tidy::ClangTidyCheck {
public:
  PreferInplaceVectorCheck(llvm::StringRef Name,
                           clang::tidy::ClangTidyContext *Context);

  bool isLanguageVersionSupported(const clang::LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus26;
  }

  void registerMatchers(clang::ast_matchers::MatchFinder *Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  /// Maximum vector size to consider for inplace_vector suggestion.
  unsigned MaxInplaceSize;
};

} // namespace checks
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_CHECKS_PREFER_INPLACE_VECTOR_CHECK_H
