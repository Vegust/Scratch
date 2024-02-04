add_executable(string_test_exec string_test.cpp ${SCRATCH_CORE_SOURCES} ../../Source/Application/Platform/Windows/windows_platform.cpp)
add_test(NAME string_test COMMAND string_test_exec)