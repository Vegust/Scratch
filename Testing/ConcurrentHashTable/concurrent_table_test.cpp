#include "../testing_shared.h"
#include "Containers/concurrent_hash_table.h"
#include <thread>

struct concurrent_table_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename key_type, typename value_type>
static bool SanityCheck(s64 Count) {
	std::cout << std::thread::hardware_concurrency();
	auto Lambda = [](s64 TestInput){
		std::cout << "!";
	};
	std::thread ThreadTest(Lambda, Count);
	ThreadTest.join();
	return true;
}

s32 concurrent_table_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<complex_type, complex_type>(100);
	return Passed ? 0 : 1;
}

void concurrent_table_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
}

TEST_ENTRY(concurrent_table_test);
