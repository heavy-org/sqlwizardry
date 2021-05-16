file(GLOB tests_src *.cpp)
add_executable(tests 
    ${tests_src}
)

target_link_libraries(tests PUBLIC
    sqlwizardry
    gtest
    gtest_main
    pthread
)

set_target_properties(tests PROPERTIES SUFFIX ".tsk" CXX_STANDARD 17)

add_test(NAME unit-tests
    COMMAND $<TARGET_FILE:tests>
    --gtest_output=xml:./xunit/tests.xunit.xml
    )