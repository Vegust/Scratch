#include "../testing_shared.h"
#include "Containers/hash_table.h"

#include <unordered_set>

struct set_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename test_type>
static bool SanityCheck(s64 Count) {
	
	std::cout << "------------------------------------------" << std::endl;
	const std::string InfoString = std::string("Testing with ") + typeid(test_type).name() + ", " +
								   std::to_string(sizeof(test_type)) + " bytes";
	std::cout << InfoString << std::endl;
	
	if constexpr (requires { test_type::NumInstances; }) {
		test_type::NumInstances = 0;
	}

	hash_set<test_type> container;
	std::unordered_set<test_type> ideal;

	for (size_t i = 0; i < Count; i++) {
		ideal.insert(MakeValue<test_type>(i));
		container.Add(MakeValue<test_type>(i));
	}

	bool Valid = true;
	for (const auto& el : ideal) {
		if (!container.Contains(el)) {
			Valid = false;
			continue;
		}
	}

	for (const auto& el : container) {
		if (!ideal.contains(el)) {
			Valid = false;
			continue;
		}
	}
	TEST_CHECK(Valid, "insertion");

	for (size_t i = 0; i < Count / 2; i++) {
		ideal.erase(MakeValue<test_type>(i * 2));
		container.RemoveOne(MakeValue<test_type>(i * 2));
	}

	for (const auto& el : ideal) {
		if (!container.Contains(el)) {
			Valid = false;
			continue;
		}
	}

	for (const auto& el : container) {
		if (!ideal.contains(el)) {
			Valid = false;
			continue;
		}
	}
	TEST_CHECK(Valid, "removal");

	container.Clear();
	ideal.clear();
	if constexpr (requires { test_type::NumInstances; }) {
		TEST_CHECK(test_type::NumInstances == 0, "object construction/destruction");
	}
	return true;
}

s32 set_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<s32>(10000);
	Passed = Passed && SanityCheck<complex_type>(10000);
	Passed = Passed && SanityCheck<complex_type_realloc>(10000);
	return Passed ? 0 : 1;
}

void set_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	std::cout << "no benchmarking for set" << std::endl;
}

TEST_ENTRY(set_test)