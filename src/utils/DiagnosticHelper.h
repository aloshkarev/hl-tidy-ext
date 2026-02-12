//===--- DiagnosticHelper.h - Diagnostic message formatting -----*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// High-Load Performance clang-tidy checks
//
// Helper utilities for building rich diagnostic messages with standard-aware
// replacement suggestions.
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_UTILS_DIAGNOSTIC_HELPER_H
#define HL_TIDY_UTILS_DIAGNOSTIC_HELPER_H

#include "CppStandardUtils.h"
#include "clang-tidy/ClangTidyCheck.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

namespace hl {
namespace tidy {
namespace utils {

/// Build a note string of the form:
///   "consider using <Alternative> (available since <Standard>)"
/// when the translation unit is compiled with at least that standard.
inline std::string
buildReplacementNote(llvm::StringRef Alternative, CppStandard Since,
                     CppStandard Current) {
  std::string Msg;
  llvm::raw_string_ostream OS(Msg);
  if (hasAtLeast(Current, Since)) {
    OS << "consider using " << Alternative
       << " (available since " << standardLabel(Since) << ")";
  } else {
    OS << Alternative << " would be a better fit but requires "
       << standardLabel(Since) << "; current standard is "
       << standardLabel(Current);
  }
  return Msg;
}

/// Build a note that simply recommends a replacement without standard
/// constraints.
inline std::string buildSimpleNote(llvm::StringRef Alternative,
                                   llvm::StringRef Reason = "") {
  std::string Msg;
  llvm::raw_string_ostream OS(Msg);
  OS << "consider using " << Alternative;
  if (!Reason.empty())
    OS << " (" << Reason << ")";
  return Msg;
}

} // namespace utils
} // namespace tidy
} // namespace hl

#endif // HL_TIDY_UTILS_DIAGNOSTIC_HELPER_H
