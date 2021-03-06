# Boilerplate lifted from range-v3
# Copyright Gonzalo Brito Gadeschi 2015
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
#
# Detects the C++ compiler, system, build-type, etc.

include(CheckCXXCompilerFlag)

if("x${CMAKE_CXX_COMPILER_ID}" MATCHES "x.*Clang")
  if("x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC")
    set (SQLWIZARDRY_CXX_COMPILER_CLANGCL TRUE)
    if (SQLWIZARDRY_VERBOSE_BUILD)
      message(STATUS "[sqlwizardry-v1]: compiler is clang-cl.")
    endif()
  else()
    set (SQLWIZARDRY_CXX_COMPILER_CLANG TRUE)
    if (SQLWIZARDRY_VERBOSE_BUILD)
      message(STATUS "[sqlwizardry-v1]: compiler is clang.")
    endif()
  endif()
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set (SQLWIZARDRY_CXX_COMPILER_GCC TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: compiler is gcc.")
  endif()
elseif("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xMSVC")
  set (SQLWIZARDRY_CXX_COMPILER_MSVC TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: compiler is msvc.")
  endif()
else()
  message(WARNING "[sqlwizardry-v1 warning]: unknown compiler ${CMAKE_CXX_COMPILER_ID} !")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set (SQLWIZARDRY_ENV_MACOSX TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: system is MacOSX.")
  endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (SQLWIZARDRY_ENV_LINUX TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: system is Linux.")
  endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set (SQLWIZARDRY_ENV_WINDOWS TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: system is Windows.")
  endif()
else()
  message(WARNING "[sqlwizardry-v1 warning]: unknown system ${CMAKE_SYSTEM_NAME} !")
endif()

if(SQLWIZARDRY_CXX_STD MATCHES "^[0-9]+$")
  if(SQLWIZARDRY_CXX_COMPILER_MSVC AND SQLWIZARDRY_CXX_STD LESS 17)
    # MSVC is currently supported only in 17+ mode
    set(SQLWIZARDRY_CXX_STD 17)
  elseif(SQLWIZARDRY_CXX_STD LESS 14)
    set(SQLWIZARDRY_CXX_STD 14)
  endif()
endif()

# Build type
set(SQLWIZARDRY_DEBUG_BUILD FALSE)
set(SQLWIZARDRY_RELEASE_BUILD FALSE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(SQLWIZARDRY_DEBUG_BUILD TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: build type is debug.")
  endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(SQLWIZARDRY_RELEASE_BUILD TRUE)
  if (SQLWIZARDRY_VERBOSE_BUILD)
    message(STATUS "[sqlwizardry-v1]: build type is release.")
  endif()
else()
  message(WARNING "[sqlwizardry-v1 warning]: unknown build type, defaults to release!")
  set(CMAKE_BUILD_TYPE "Release")
  set(SQLWIZARDRY_RELEASE_BUILD TRUE)
endif()