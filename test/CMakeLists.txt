add_subdirectory(unit)
#dd_subdirectory(benchmark)

if(ENABLE_TEST_COVERAGE)
    target_compile_options(tests.hh.tsk PUBLIC --coverage -O0 -g -fprofile-arcs
        -ftest-coverage -fno-inline -fno-inline-small-functions
        -fno-default-inline)
    target_link_options(tests.hh.tsk PUBLIC --coverage -fprofile-arcs -ftest-coverage
        -fno-inline -fno-inline-small-functions -fno-default-inline)
    message(STATUS "Compiling with coverage flags.")
endif()