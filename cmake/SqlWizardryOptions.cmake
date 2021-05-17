# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# CMake options

include(CMakeDependentOption)

set(SQLWIZARDRY_CXX_STD default CACHE STRING "C++ standard version.")
option(SQLWIZARDRY_ASAN "Run the tests using AddressSanitizer." OFF)
option(SQLWIZARDRY_MSAN "Run the tests using MemorySanitizer." OFF)
option(SQLWIZARDRY_ENABLE_TEST_COVERAGE "Run the tests using MemorySanitizer." OFF)
option(SQLWIZARDRY_ASSERTIONS "Enable assertions." ON)
option(SQLWIZARDRY_DEBUG_INFO "Include debug information in the binaries." ON)
option(SQLWIZARDRY_MODULES "Enables use of Clang modules (experimental)." OFF)
option(SQLWIZARDRY_NATIVE "Enables -march/-mtune=native." ON)
option(SQLWIZARDRY_VERBOSE_BUILD "Enables debug output from CMake." OFF)
option(SQLWIZARDRY_LLVM_POLLY "Enables LLVM Polly." OFF)
option(SQLWIZARDRY_ENABLE_WERROR
  "Enables -Werror. Only effective if compiler is not clang-cl or MSVC. ON by default"
  ON)
set(SQLWIZARDRY_INLINE_THRESHOLD -1 CACHE STRING "Force a specific inlining threshold.")

# Enable verbose configure when passing -Wdev to CMake
if (DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS AND
    NOT CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(SQLWIZARDRY_VERBOSE_BUILD ON)
endif()

if (SQLWIZARDRY_VERBOSE_BUILD)
  message(STATUS "[sqlwizardry]: verbose build enabled.")
endif()

CMAKE_DEPENDENT_OPTION(SQLWIZARDRY_TESTS
  "Build the SqlWizardry tests and integrate with ctest"
  ON "${is_standalone}" OFF)

option(SQLWIZARDRY_PERF
  "Build the SqlWizardry performance benchmarks"
  OFF)

CMAKE_DEPENDENT_OPTION(SQLWIZARDRY_DOCS
  "Build the SqlWizardry-v1 documentation"
  ON "${is_standalone}" OFF)

mark_as_advanced(SQLWIZARDRY_PERF)