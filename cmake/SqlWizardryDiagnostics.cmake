# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in
# subdirectories)

# Enable all warnings:
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WEVERYTHING -Weverything)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_PEDANTIC -pedantic)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_PEDANTIC_ERRORS -pedantic-errors)

# Selectively disable those warnings that are not worth it:
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_CXX98_COMPAT -Wno-c++98-compat)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_CXX98_COMPAT_PEDANTIC -Wno-c++98-compat-pedantic)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_WEAK_VTABLES -Wno-weak-vtables)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_PADDED -Wno-padded)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_MISSING_VARIABLE_DECLARATIONS -Wno-missing-variable-declarations)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_DOCUMENTATION -Wno-documentation)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_DOCUMENTATION_UNKNOWN_COMMAND -Wno-documentation-unknown-command)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_OLD_STYLE_CAST -Wno-old-style-cast)

if (SQLWIZARDRY_ENV_MACOSX)
  sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_GLOBAL_CONSTRUCTORS -Wno-global-constructors)
  sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_EXIT_TIME_DESTRUCTORS -Wno-exit-time-destructors)
endif()

if (SQLWIZARDRY_CXX_COMPILER_CLANG OR SQLWIZARDRY_CXX_COMPILER_CLANGCL)
  sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_MISSING_PROTOTYPES -Wno-missing-prototypes)
endif()

if (SQLWIZARDRY_CXX_COMPILER_GCC)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
    sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_STRICT_OVERFLOW -Wno-strict-overflow)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0")
      sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_MISSING_FIELD_INITIALIZERS -Wno-missing-field-initializers)
    endif()
  elseif ((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0") OR (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "7.0"))
    sqlwizardry_append_flag(SQLWIZARDRY_HAS_WNO_NOEXCEPT_TYPE -Wno-noexcept-type)
  endif()
endif()

if (SQLWIZARDRY_VERBOSE_BUILD)
  message(STATUS "[sqlwizardry]: test C++ flags: ${CMAKE_CXX_FLAGS}")
endif()