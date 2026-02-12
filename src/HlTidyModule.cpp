//===--- HlTidyModule.cpp - High-Load Performance module ------*- C++ -*-===//
// Author: Aleksandr Loshkarev
//
// Registers all hl-* checks with clang-tidy.
//
// Check naming convention:
//   hl-perf-*       — core performance checks (all C++ standards)
//   hl-modernize-*  — standard-upgrade suggestions (C++20/23/26)
//
//===----------------------------------------------------------------------===//

#include "HlTidyModule.h"

// Core performance checks (C++17 baseline).
#include "checks/AvoidCoutCerrCheck.h"
#include "checks/AvoidDynamicCastCheck.h"
#include "checks/AvoidStdAnyCheck.h"
#include "checks/AvoidStdBindCheck.h"
#include "checks/AvoidStdEndlCheck.h"
#include "checks/AvoidStdFunctionCheck.h"
#include "checks/AvoidStdRegexCheck.h"
#include "checks/AvoidVirtualInLoopCheck.h"
#include "checks/PreferEmplaceCheck.h"
#include "checks/PreferFromCharsCheck.h"
#include "checks/PreferNoexceptMoveCheck.h"
#include "checks/PreferReserveCheck.h"
#include "checks/PreferStringViewCheck.h"
#include "checks/PreferUniquePtrCheck.h"
#include "checks/PreferVectorOverListCheck.h"

// C++20 modernisation checks.
#include "checks/PreferContainsCheck.h"
#include "checks/PreferEraseIfCheck.h"
#include "checks/PreferFormatCheck.h"
#include "checks/PreferJthreadCheck.h"
#include "checks/PreferSpanCheck.h"
#include "checks/PreferStartsEndsWithCheck.h"

// C++23 modernisation checks.
#include "checks/PreferExpectedCheck.h"
#include "checks/PreferFlatContainersCheck.h"
#include "checks/PreferMoveOnlyFunctionCheck.h"
#include "checks/PreferPrintCheck.h"
#include "checks/PreferToUnderlyingCheck.h"
#include "checks/PreferUnreachableCheck.h"

// C++26 modernisation checks.
#include "checks/PreferCopyableFunctionCheck.h"
#include "checks/PreferFunctionRefCheck.h"
#include "checks/PreferHiveCheck.h"
#include "checks/PreferInplaceVectorCheck.h"

namespace hl {
namespace tidy {

void HlTidyModule::addCheckFactories(
    clang::tidy::ClangTidyCheckFactories &CheckFactories) {

  // -----------------------------------------------------------------------
  // Core performance checks — always relevant for high-load services.
  // -----------------------------------------------------------------------
  CheckFactories.registerCheck<checks::AvoidStdFunctionCheck>(
      "hl-perf-avoid-std-function");
  CheckFactories.registerCheck<checks::AvoidStdRegexCheck>(
      "hl-perf-avoid-std-regex");
  CheckFactories.registerCheck<checks::AvoidStdEndlCheck>(
      "hl-perf-avoid-std-endl");
  CheckFactories.registerCheck<checks::PreferVectorOverListCheck>(
      "hl-perf-prefer-vector");
  CheckFactories.registerCheck<checks::PreferStringViewCheck>(
      "hl-perf-prefer-string-view");
  CheckFactories.registerCheck<checks::PreferFromCharsCheck>(
      "hl-perf-prefer-from-chars");
  CheckFactories.registerCheck<checks::AvoidStdAnyCheck>(
      "hl-perf-avoid-std-any");
  CheckFactories.registerCheck<checks::PreferUniquePtrCheck>(
      "hl-perf-prefer-unique-ptr");
  CheckFactories.registerCheck<checks::PreferReserveCheck>(
      "hl-perf-prefer-reserve");
  CheckFactories.registerCheck<checks::AvoidDynamicCastCheck>(
      "hl-perf-avoid-dynamic-cast");
  CheckFactories.registerCheck<checks::AvoidStdBindCheck>(
      "hl-perf-avoid-std-bind");
  CheckFactories.registerCheck<checks::PreferEmplaceCheck>(
      "hl-perf-prefer-emplace");
  CheckFactories.registerCheck<checks::PreferNoexceptMoveCheck>(
      "hl-perf-prefer-noexcept-move");
  CheckFactories.registerCheck<checks::AvoidCoutCerrCheck>(
      "hl-perf-avoid-cout-cerr");
  CheckFactories.registerCheck<checks::AvoidVirtualInLoopCheck>(
      "hl-perf-avoid-virtual-in-loop");

  // -----------------------------------------------------------------------
  // C++20 modernisation — active only when -std=c++20 or later.
  // -----------------------------------------------------------------------
  CheckFactories.registerCheck<checks::PreferJthreadCheck>(
      "hl-modernize-prefer-jthread");
  CheckFactories.registerCheck<checks::PreferFormatCheck>(
      "hl-modernize-prefer-format");
  CheckFactories.registerCheck<checks::PreferSpanCheck>(
      "hl-modernize-prefer-span");
  CheckFactories.registerCheck<checks::PreferStartsEndsWithCheck>(
      "hl-modernize-prefer-starts-ends-with");
  CheckFactories.registerCheck<checks::PreferContainsCheck>(
      "hl-modernize-prefer-contains");
  CheckFactories.registerCheck<checks::PreferEraseIfCheck>(
      "hl-modernize-prefer-erase-if");

  // -----------------------------------------------------------------------
  // C++23 modernisation — active only when -std=c++23 or later.
  // -----------------------------------------------------------------------
  CheckFactories.registerCheck<checks::PreferExpectedCheck>(
      "hl-modernize-prefer-expected");
  CheckFactories.registerCheck<checks::PreferFlatContainersCheck>(
      "hl-modernize-prefer-flat-containers");
  CheckFactories.registerCheck<checks::PreferMoveOnlyFunctionCheck>(
      "hl-modernize-prefer-move-only-function");
  CheckFactories.registerCheck<checks::PreferUnreachableCheck>(
      "hl-modernize-prefer-unreachable");
  CheckFactories.registerCheck<checks::PreferToUnderlyingCheck>(
      "hl-modernize-prefer-to-underlying");
  CheckFactories.registerCheck<checks::PreferPrintCheck>(
      "hl-modernize-prefer-print");

  // -----------------------------------------------------------------------
  // C++26 modernisation — active only when -std=c++26 or later.
  // -----------------------------------------------------------------------
  CheckFactories.registerCheck<checks::PreferFunctionRefCheck>(
      "hl-modernize-prefer-function-ref");
  CheckFactories.registerCheck<checks::PreferInplaceVectorCheck>(
      "hl-modernize-prefer-inplace-vector");
  CheckFactories.registerCheck<checks::PreferCopyableFunctionCheck>(
      "hl-modernize-prefer-copyable-function");
  CheckFactories.registerCheck<checks::PreferHiveCheck>(
      "hl-modernize-prefer-hive");
}

} // namespace tidy
} // namespace hl

// Register the module with the clang-tidy plugin infrastructure.
// The "hl-" prefix means all checks are accessible as hl-perf-* and
// hl-modernize-* in .clang-tidy configuration.
static clang::tidy::ClangTidyModuleRegistry::Add<hl::tidy::HlTidyModule>
    X("hl-module", "High-Load Performance checks for C++ services");

// Force the linker to keep the module registration.
// This is required for the plugin to be discoverable when loaded with -load.
volatile int HlTidyModuleAnchorSource = 0;
