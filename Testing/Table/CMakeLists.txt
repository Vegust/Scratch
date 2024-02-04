add_executable(table_test_exec table_test.cpp)
target_link_libraries(table_test_exec ScratchLib)
add_test(NAME table_test COMMAND table_test_exec)
add_test(NAME table_benchmark COMMAND table_test_exec --benchmark)