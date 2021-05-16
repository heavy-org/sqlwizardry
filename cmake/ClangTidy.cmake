
function(clang_format input_files)
find_program(CLANG_TIDY NAMES "clang-tidy")
    if(CLANG_TIDY)
        message(STATUS "Found clang-tidy at ${CLANG_TIDY}")
        execute_process(${CLANG_TIDY}
                COMMAND ${CLANG_TIDY} -p
                ${PROJECT_BINARY_DIR}/compile_commands.json --checks=all
                ${SRCS})
    else()
        message(WARNING "Unable to find clang-format executable, will skip formatting enforcement.")
    endif()
endfunction()