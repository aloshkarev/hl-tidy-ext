# Author: Aleksandr Loshkarev
# Author: Aleksandr Loshkarev
# Author: Aleksandr Loshkarev
cmake_minimum_required(VERSION 3.16)
project(HlTidyExt
  VERSION 1.0.0
  DESCRIPTION "High-Load Performance clang-tidy checks"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------------- #
# Find LLVM / Clang
# --------------------------------------------------------------------------- #
# Allow specifying LLVM version, e.g.: cmake -DLLVM_VERSION=20 ..
set(LLVM_VERSION "" CACHE STRING "LLVM version to use (e.g., 20)")

if(LLVM_VERSION)
  set(LLVM_DIR "/usr/lib/llvm-${LLVM_VERSION}/lib/cmake/llvm" CACHE PATH "" FORCE)
  set(Clang_DIR "/usr/lib/llvm-${LLVM_VERSION}/lib/cmake/clang" CACHE PATH "" FORCE)
endif()

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# --------------------------------------------------------------------------- #
# Include paths
# --------------------------------------------------------------------------- #
# clang-tidy headers live under LLVM include dir (e.g. /usr/lib/llvm-20/include)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# --------------------------------------------------------------------------- #
# Plugin shared library
# --------------------------------------------------------------------------- #
add_library(HlTidyModule MODULE
  # Module registration
  src/HlTidyModule.cpp

  # Core performance checks (all standards)
  src/checks/AvoidCoutCerrCheck.cpp
  src/checks/AvoidDynamicCastCheck.cpp
  src/checks/AvoidStdAnyCheck.cpp
  src/checks/AvoidStdBindCheck.cpp
  src/checks/AvoidStdEndlCheck.cpp
  src/checks/AvoidStdFunctionCheck.cpp
  src/checks/AvoidStdRegexCheck.cpp
  src/checks/AvoidVirtualInLoopCheck.cpp
  src/checks/PreferEmplaceCheck.cpp
  src/checks/PreferFromCharsCheck.cpp
  src/checks/PreferNoexceptMoveCheck.cpp
  src/checks/PreferReserveCheck.cpp
  src/checks/PreferStringViewCheck.cpp
  src/checks/PreferUniquePtrCheck.cpp
  src/checks/PreferVectorOverListCheck.cpp

  # C++20 modernisation checks
  src/checks/PreferContainsCheck.cpp
  src/checks/PreferEraseIfCheck.cpp
  src/checks/PreferFormatCheck.cpp
  src/checks/PreferJthreadCheck.cpp
  src/checks/PreferSpanCheck.cpp
  src/checks/PreferStartsEndsWithCheck.cpp

  # C++23 modernisation checks
  src/checks/PreferExpectedCheck.cpp
  src/checks/PreferFlatContainersCheck.cpp
  src/checks/PreferMoveOnlyFunctionCheck.cpp
  src/checks/PreferPrintCheck.cpp
  src/checks/PreferToUnderlyingCheck.cpp
  src/checks/PreferUnreachableCheck.cpp

  # C++26 modernisation checks
  src/checks/PreferCopyableFunctionCheck.cpp
  src/checks/PreferFunctionRefCheck.cpp
  src/checks/PreferHiveCheck.cpp
  src/checks/PreferInplaceVectorCheck.cpp
)

target_include_directories(HlTidyModule PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# On Linux we need to link clangTidy* statically; on macOS undefined symbols
# are resolved at load time.
if(NOT APPLE)
  target_link_libraries(HlTidyModule PRIVATE
    clangTidy
    clangTidyUtils
    clangASTMatchers
    clangTooling
    clangBasic
    clangAST
    clangLex
  )
else()
  # Let symbols be resolved at load time
  set_target_properties(HlTidyModule PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
  )
endif()

target_compile_options(HlTidyModule PRIVATE
  -fno-rtti            # LLVM is built without RTTI
  -fno-exceptions
)

set_target_properties(HlTidyModule PROPERTIES
  PREFIX ""            # produce HlTidyModule.so, not libHlTidyModule.so
  POSITION_INDEPENDENT_CODE ON
)

# --------------------------------------------------------------------------- #
# Install
# --------------------------------------------------------------------------- #
install(TARGETS HlTidyModule
  LIBRARY DESTINATION lib/clang-tidy/plugins
)

# --------------------------------------------------------------------------- #
# Tests (optional – requires lit)
# --------------------------------------------------------------------------- #
option(HL_TIDY_ENABLE_TESTS "Enable lit-based tests" OFF)
if(HL_TIDY_ENABLE_TESTS)
  add_subdirectory(test)
endif()
