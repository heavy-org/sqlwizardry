cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
get_directory_property(is_subproject PARENT_DIRECTORY)

if(NOT is_subproject)
    set(is_standalone YES)
else()
    set(is_standalone NO)
endif()



if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new
    directory (called a build directory) and run CMake from there.")
endif()

project(SqlWizardry CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compilation data-base
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(SqlWizardryOptions)
include(SqlWizardryEnv)
include(SqlWizardryFlags)
include(ClangFormat)
include(ClangTidy)

file( GLOB_RECURSE SRCS *.cpp *.h )

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost 1.45.0) 

include_directories(${Boost_INCLUDE_DIRS}) 
add_library(sqlwizardry INTERFACE)
target_compile_features(sqlwizardry INTERFACE cxx_std_17)
add_library(sqlwizardry::sqlwizardry ALIAS sqlwizardry)
target_include_directories(sqlwizardry INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(sqlwizardry SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)
target_compile_options(sqlwizardry INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)

if(SQLWIZARDRY_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

if(SQLWIZARDRY_DOCS)
    # setup venv if it doesn't exist
    IF (NOT EXISTS "${CMAKE_BINARY_DIR}/venv")
        FIND_PACKAGE(Python3)
        EXECUTE_PROCESS(COMMAND ${Python3_EXECUTABLE} "-m" "venv" "venv"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        UNSET(${Python3_EXECUTABLE})
    ENDIF ()
    # activate python venv
    SET(ENV{VIRTUAL_ENV} "${CMAKE_BINARY_DIR}/venv")
    set (Python3_FIND_VIRTUALENV FIRST)
    ## unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
    unset (Python3_EXECUTABLE)
    ## Launch a new search
    find_package (Python3 3.8 COMPONENTS Interpreter Development)
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install -r ${CMAKE_SOURCE_DIR}/requirements.txt)
    execute_process(COMMAND ${Python3_EXECUTABLE} -m mkdocs build WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

add_custom_target(tar ALL COMMAND
    ${CMAKE_COMMAND} -E tar "cfvzP" "${PROJECT_NAME}-${PROJECT_VERSION}.tar.gz" "${EXECUTABLE_OUTPUT_PATH}")
add_dependencies(tar sqlwizardry)

set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
foreach(LIB ${COMMA_SEAPARATED_LIST})
  set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
endforeach()

CONFIGURE_FILE("sqlwizardry.pc.in" "sqlwizardry.pc" @ONLY)