file(GLOB test_srcs CONFIGURE_DEPENDS  *.cpp)

add_executable(tests
    ${test_srcs}
)

target_link_libraries(tests PUBLIC
    sqlwizardry
    gtest
    gtest_main
    gmock
    pthread 
)

set_target_properties(tests PROPERTIES
    SUFFIX ".tsk"
    CXX_STANDARD 11
)

add_test(NAME unit-tests
    COMMAND $<TARGET_FILE:tests>
    --gtest_output=xml:./xunit/tests.xunit.xml
    )