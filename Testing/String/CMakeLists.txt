add_executable(string_test_exec string_test.cpp ${SCRATCH_CORE_SOURCES})
add_test(NAME string_test COMMAND string_test_exec)