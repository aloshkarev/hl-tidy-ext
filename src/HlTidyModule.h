//===--- HlTidyModule.h - High-Load Performance module --------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Module registration header for the hl-tidy clang-tidy plugin.
// This module provides performance-oriented checks for high-load C++ services.
//
//===----------------------------------------------------------------------===//

#ifndef HL_TIDY_MODULE_H
#define HL_TIDY_MODULE_H

#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"

namespace hl {
namespace tidy {

class HlTidyModule : public clang::tidy::ClangTidyModule {
public:
  void addCheckFactories(
      clang::tidy::ClangTidyCheckFactories &CheckFactories) override;
};

} // namespace tidy
} // namespace hl

#endif // HL_TIDY_MODULE_H
