add_executable(tree_test_exec tree_test.cpp)
target_link_libraries(tree_test_exec ScratchLib)
add_test(NAME tree_test COMMAND tree_test_exec)
add_test(NAME tree_benchmark COMMAND tree_test_exec --benchmark)