//===--- CppStandardUtils.h - C++ standard detection utilities --*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// High-Load Performance clang-tidy checks
//
// Utility helpers for detecting the C++ standard level of the translation unit
// being analysed and providing standard-aware diagnostic recommendations.
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_UTILS_CPP_STANDARD_UTILS_H
#define HL_TIDY_UTILS_CPP_STANDARD_UTILS_H

#include "clang/AST/ASTContext.h"
#include "clang/Basic/LangOptions.h"
#include "llvm/ADT/StringRef.h"

namespace hl {
namespace tidy {
namespace utils {

/// Logical C++ standard level used throughout the checks.
enum class CppStandard : unsigned {
  Cpp17 = 17,
  Cpp20 = 20,
  Cpp23 = 23,
  Cpp26 = 26,
  Unknown = 0
};

/// Detect the C++ standard from the LangOptions attached to the ASTContext.
inline CppStandard detectStandard(const clang::ASTContext &Ctx) {
  const auto &LO = Ctx.getLangOpts();
  // LangStandard comparison – clang exposes individual booleans.
  if (LO.CPlusPlus26)
    return CppStandard::Cpp26;
  if (LO.CPlusPlus23)
    return CppStandard::Cpp23;
  if (LO.CPlusPlus20)
    return CppStandard::Cpp20;
  if (LO.CPlusPlus17)
    return CppStandard::Cpp17;
  return CppStandard::Unknown;
}

/// Return true when the translation unit is compiled with at least \p Min.
inline bool hasAtLeast(CppStandard Current, CppStandard Min) {
  return static_cast<unsigned>(Current) >= static_cast<unsigned>(Min);
}

/// Human-readable label, e.g. "C++20".
inline llvm::StringRef standardLabel(CppStandard S) {
  switch (S) {
  case CppStandard::Cpp17:
    return "C++17";
  case CppStandard::Cpp20:
    return "C++20";
  case CppStandard::Cpp23:
    return "C++23";
  case CppStandard::Cpp26:
    return "C++26";
  default:
    return "unknown";
  }
}

} // namespace utils
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_UTILS_CPP_STANDARD_UTILS_H
