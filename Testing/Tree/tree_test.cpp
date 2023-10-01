#include "../testing_shared.h"
#include "Containers/rb_set.h"

#include <set>

struct tree_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename test_type>
static void PerformanceTests(s64 Count, s64 Iters) {
	std::cout << "------------------------------------------" << std::endl;
	const std::string InfoString = std::string("Testing with ") + typeid(test_type).name() + ", " +
								   std::to_string(sizeof(test_type)) + " bytes.";
	std::cout << InfoString << std::endl;

	timer stdSetInsert;
	timer rbSetInsert;
	timer stdSetRemove;
	timer rbSetRemove;

	for (s32 iter = 0; iter < Iters; ++iter) {
		rb_set<test_type> container;
		std::set<test_type> ideal;

		std::vector<test_type> Values;

		srand(0);
		for (size_t i = 0; i < Count; i++) {
			const int32_t value = rand();
			Values.push_back(value);
		}

		ClearCache();
		srand(0);
		rbSetInsert.Start();
		for (size_t i = 0; i < Count; i++) {
			const int32_t value = rand();
			container.AddUnique(value);
		}
		rbSetInsert.Stop();

		ClearCache();
		srand(0);
		stdSetInsert.Start();
		for (size_t i = 0; i < Count; i++) {
			const int32_t value = rand();
			ideal.insert(value);
		}
		stdSetInsert.Stop();

		uint32_t countToRemove = Count / 2;
		ClearCache();
		srand(0);
		stdSetRemove.Start();
		for (size_t i = 0; i < countToRemove; i++) {
			const size_t pos = rand() % Values.size();
			const auto valueToRemove = Values[pos];
			ideal.erase(valueToRemove);
		}
		stdSetRemove.Stop();

		ClearCache();
		srand(0);
		rbSetRemove.Start();
		for (size_t i = 0; i < countToRemove; i++) {
			const size_t pos = rand() % Values.size();
			const auto valueToRemove = Values[pos];
			container.Remove(valueToRemove);
		}
		rbSetRemove.Stop();
	}

	std::cout << std::endl;

	std::cout << "Performance test insert:\n\tstd::set " << stdSetInsert.Result() << " ms"
			  << "\n\trb_set " << rbSetInsert.Result() << " ms" << std::endl;
	std::cout << "Performance test remove:\n\tstd::set " << stdSetRemove.Result() << " ms"
			  << "\n\trb_set " << rbSetRemove.Result() << " ms" << std::endl;
}

template <typename test_type>
static bool SanityCheck(s64 Count) {
	std::cout << "------------------------------------------" << std::endl;
	const std::string InfoString = std::string("Testing with ") + typeid(test_type).name() + ", " +
								   std::to_string(sizeof(test_type)) + " bytes";
	std::cout << InfoString << std::endl;

	if constexpr (requires { test_type::NumInstances; }) {
		test_type::NumInstances = 0;
	}

	rb_set<test_type> container;
	std::set<test_type> ideal;

	for (size_t i = 0; i < Count; i++) {
		ideal.insert(MakeValue<test_type>(i));
		container.AddUnique(MakeValue<test_type>(i));
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
		container.Remove(MakeValue<test_type>(i * 2));
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

s32 tree_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<s32>(10000);
	Passed = Passed && SanityCheck<s32>(10000);
	Passed = Passed && SanityCheck<bytes_struct<16>>(10000);
	Passed = Passed && SanityCheck<complex_type>(10000);
	Passed = Passed && SanityCheck<complex_type_realloc>(10000);
	return Passed ? 0 : 1;
}

void tree_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	PerformanceTests<s32>(10000, 1000);
	PerformanceTests<bytes_struct<128>>(10000, 1000);
	PerformanceTests<complex_type>(10000, 1000);
}

TEST_ENTRY(tree_test);
