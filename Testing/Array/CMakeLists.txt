add_executable(array_test_exec array_test.cpp)
target_link_libraries(array_test_exec ScratchLib)
add_test(NAME array_test COMMAND array_test_exec)
add_test(NAME array_benchmark COMMAND array_test_exec --benchmark)