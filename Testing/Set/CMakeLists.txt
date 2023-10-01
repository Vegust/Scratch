add_executable(set_test_exec set_test.cpp ${SCRATCH_CORE_SOURCES})
add_test(NAME set_test COMMAND set_test_exec)