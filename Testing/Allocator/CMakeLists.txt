add_executable(allocator_test_exec allocator_test.cpp ${SCRATCH_CORE_SOURCES})
add_test(NAME allocator_test COMMAND allocator_test_exec)
add_test(NAME allocator_benchmark COMMAND allocator_test_exec --benchmark)