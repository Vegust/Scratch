add_executable(json_test_exec json_test.cpp)
target_link_libraries(json_test_exec ScratchLib)
add_test(NAME json_test_test COMMAND json_test_exec)
