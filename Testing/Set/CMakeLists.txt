add_executable(set_test_exec set_test.cpp)
target_link_libraries(set_test_exec ScratchLib)
add_test(NAME set_test COMMAND set_test_exec)