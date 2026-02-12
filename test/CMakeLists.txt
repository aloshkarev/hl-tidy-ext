# Tests for hl-tidy-ext checks using LLVM lit framework.
#
# Requires: llvm-lit, FileCheck
#
# Usage:
#   cmake -DHL_TIDY_ENABLE_TESTS=ON ..
#   cmake --build .
#   llvm-lit test/

find_program(LIT_COMMAND NAMES llvm-lit lit)
find_program(FILECHECK_COMMAND NAMES FileCheck)

if(NOT LIT_COMMAND)
  message(WARNING "llvm-lit not found; tests will not run")
  return()
endif()

if(NOT FILECHECK_COMMAND)
  message(WARNING "FileCheck not found; tests will not run")
  return()
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.cfg.py
  @ONLY
)

# Copy test source files to build directory.
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(TEST_FILE ${TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME)
  configure_file(${TEST_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME} COPYONLY)
endforeach()

add_custom_target(check-hl-tidy
  COMMAND ${LIT_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS HlTidyModule
  COMMENT "Running hl-tidy tests..."
)
