# Lifted boilerplate from range-v3 library.
# Copyright Louis Dionne 2015
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Setup compiler flags (more can be set on a per-target basis or in
# subdirectories)

include(CheckCXXCompilerFlag)
macro(sqlwizardry_append_flag testname flag)
  # As -Wno-* flags do not lead to build failure when there are no other
  # diagnostics, we check positive option to determine their applicability.
  # Of course, we set the original flag that is requested in the parameters.
  string(REGEX REPLACE "^-Wno-" "-W" alt ${flag})
  check_cxx_compiler_flag(${alt} ${testname})
  if (${testname})
    add_compile_options(${flag})
  endif()
endmacro()

function(cxx_standard_normalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x1y")
    set( ${return_value} "14" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "x1z")
    set( ${return_value} "17" PARENT_SCOPE )
  elseif("x${cxx_standard}" STREQUAL "xlatest" OR "x${cxx_standard}" STREQUAL "x2a")
    set( ${return_value} "20" PARENT_SCOPE )
  else()
    set( ${return_value} "${cxx_standard}" PARENT_SCOPE )
  endif()
endfunction()

function(cxx_standard_denormalize cxx_standard return_value)
  if("x${cxx_standard}" STREQUAL "x17")
    if (SQLWIZARDRY_CXX_COMPILER_CLANGCL OR SQLWIZARDRY_CXX_COMPILER_MSVC)
      set( ${return_value} 17 PARENT_SCOPE )
    else()
      set( ${return_value} 1z PARENT_SCOPE )
    endif()
  elseif("x${cxx_standard}" STREQUAL "x20")
    if (SQLWIZARDRY_CXX_COMPILER_CLANGCL OR SQLWIZARDRY_CXX_COMPILER_MSVC)
      set( ${return_value} latest PARENT_SCOPE )
    else()
      set( ${return_value} 2a PARENT_SCOPE )
    endif()
  else()
    set( ${return_value} ${cxx_standard} PARENT_SCOPE )
  endif()
endfunction()

if(CMAKE_CXX_STANDARD)
  if(NOT "x${SQLWIZARDRY_CXX_STD}" STREQUAL "xdefault")
    # Normalize SQLWIZARDRY_CXX_STD
    cxx_standard_normalize( ${SQLWIZARDRY_CXX_STD} ranges_cxx_std )
    if(NOT "x${ranges_cxx_std}" STREQUAL "x${CMAKE_CXX_STANDARD}")
      message(FATAL_ERROR "[sqlwizardry-v1]: Cannot specify both CMAKE_CXX_STANDARD and SQLWIZARDRY_CXX_STD, or they must match.")
    endif()
  else()
    cxx_standard_denormalize(${CMAKE_CXX_STANDARD} SQLWIZARDRY_CXX_STD)
  endif()
elseif("x${SQLWIZARDRY_CXX_STD}" STREQUAL "xdefault")
  if (SQLWIZARDRY_CXX_COMPILER_CLANGCL OR SQLWIZARDRY_CXX_COMPILER_MSVC)
    set(SQLWIZARDRY_CXX_STD 17)
  else()
    set(SQLWIZARDRY_CXX_STD 14)
  endif()
endif()

sqlwizardry_append_flag(SQLWIZARDRY_HAS_FDIAGNOSTIC_SHOW_TEMPLATE_TREE -fdiagnostics-show-template-tree)
sqlwizardry_append_flag(SQLWIZARDRY_HAS_FTEMPLATE_BACKTRACE_LIMIT "-ftemplate-backtrace-limit=0")
sqlwizardry_append_flag(SQLWIZARDRY_HAS_FMACRO_BACKTRACE_LIMIT "-fmacro-backtrace-limit=1")