#include "../testing_shared.h"
#include "Containers/dyn_array.h"

struct array_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename T>
static bool Compare(const std::vector<T>& lhs, const dyn_array<T>& rhs) {
	if (lhs.size() != rhs.Size()) {
		return false;
	}

	for (size_t i = 0; i < lhs.size(); i++) {
		if (rhs[i] != lhs[i]) {
			return false;
		}
	}
	return true;
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
	dyn_array<test_type> container;
	std::vector<test_type> ideal;
	srand(0);
	for (size_t i = 0; i < Count; i++) {
		const int32_t value = rand();
		if (i % 2 == 0) {
			ideal.push_back(MakeValue<test_type>(value));
			container.Add(MakeValue<test_type>(value));
		} else {
			ideal.emplace_back(MakeValue<test_type>(value));
			container.Emplace(MakeValue<test_type>(value));
		}
	}
	TEST_CHECK(Compare(ideal, container), "add");
	uint32_t countToDelete = Count / 2;
	for (size_t i = 0; i < countToDelete; i++) {
		const int32_t value = rand() % ideal.size();
		ideal.erase(ideal.begin() + value);
		container.RemoveAt(value);
	}
	TEST_CHECK(Compare(ideal, container), "remove");
	uint32_t countToAdd = Count / 4;
	for (size_t i = 0; i < countToAdd; i++) {
		const size_t num = rand() % 16;
		const size_t pos = rand() % ideal.size();

		ideal.insert(ideal.begin() + pos, 1u, MakeValue<test_type>(i));
		container.EmplaceAt(pos, MakeValue<test_type>(i));
	}
	TEST_CHECK(Compare(ideal, container), "emplace");
	uint32_t countToRemoveSwap = Count / 16;
	for (size_t i = 0; i < countToRemoveSwap; i++) {
		const size_t pos = rand() % ideal.size();
		const auto valueToRemove = ideal[pos];

		std::iter_swap(ideal.begin() + pos, ideal.end() - 1);
		ideal.pop_back();

		container.RemoveAtSwap(pos);
	}
	TEST_CHECK(Compare(ideal, container), "remove swap");
	std::sort(ideal.begin(), ideal.end());
	container.Sort();
	TEST_CHECK(Compare(ideal, container), "sort");
	container.Clear();
	ideal.clear();
	if constexpr (requires { test_type::NumInstances; }) {
		TEST_CHECK(test_type::NumInstances == 0, "object construction/destruction");
	}
	return true;
}

template <typename test_type>
static void PerformanceTests(s64 Count, s64 Iters) {
	std::cout << "------------------------------------------" << std::endl;
	const std::string InfoString = std::string("Testing with ") + typeid(test_type).name() + ", " +
								   std::to_string(sizeof(test_type)) + " bytes";
	std::cout << InfoString << std::endl;

	timer stdVector;
	timer tVector;
	timer stdVectorRemoveSwap;
	timer tVectorRemoveSwap;
	timer stdVectorRemoveAt;
	timer tVectorRemoveAt;
	timer stdVectorInsert;
	timer tVectorInsert;
	timer stdVectorSort;
	timer tVectorSort;

	for (s32 iter = 0; iter < Iters; ++iter) {
		dyn_array<test_type> container;
		std::vector<test_type> ideal;

		ClearCache();
		srand(0);
		tVector.Start();
		for (size_t i = 0; i < Count; i++) {
			const int32_t value = rand();

			if (i % 2 == 0) {
				container.Add(value);
			} else {
				container.Emplace(value);
			}
		}
		tVector.Stop();

		ClearCache();
		srand(0);
		stdVector.Start();
		for (size_t i = 0; i < Count; i++) {
			const int32_t value = rand();

			if (i % 2 == 0) {
				ideal.push_back(value);
			} else {
				ideal.emplace_back(value);
			}
		}
		stdVector.Stop();

		ClearCache();
		tVectorSort.Start();
		container.Sort();
		tVectorSort.Stop();

		ClearCache();
		stdVectorSort.Start();
		std::sort(ideal.begin(), ideal.end());
		stdVectorSort.Stop();

		uint32_t countToRemoveSwap = Count / 2;

		ClearCache();
		srand(0);
		stdVectorRemoveSwap.Start();
		for (size_t i = 0; i < countToRemoveSwap; i++) {
			const size_t pos = rand() % ideal.size();
			const auto valueToRemove = ideal[pos];

			std::iter_swap(ideal.begin() + pos, ideal.end() - 1);
			ideal.pop_back();
		}
		stdVectorRemoveSwap.Stop();

		ClearCache();
		srand(0);
		tVectorRemoveSwap.Start();
		for (size_t i = 0; i < countToRemoveSwap; i++) {
			const size_t pos = rand() % container.Size();
			const auto valueToRemove = container[pos];

			container.RemoveAtSwap(pos);
		}
		tVectorRemoveSwap.Stop();

		uint32_t countToDelete = Count / 128;

		ClearCache();
		srand(0);
		tVectorRemoveAt.Start();
		for (size_t i = 0; i < countToDelete; i++) {
			const int32_t value = rand() % container.Size();
			container.RemoveAt(value);
		}
		tVectorRemoveAt.Stop();

		ClearCache();
		srand(0);
		stdVectorRemoveAt.Start();
		for (size_t i = 0; i < countToDelete; i++) {
			const int32_t value = rand() % ideal.size();
			ideal.erase(ideal.begin() + value);
		}
		stdVectorRemoveAt.Stop();

		uint32_t countToAdd = Count / 256;

		ClearCache();
		tVectorInsert.Start();
		for (size_t i = 0; i < countToAdd; i++) {
			const size_t num = rand() % 16;
			const size_t pos = rand() % container.Size();
			container.EmplaceAt(pos, test_type((uint32_t) i));
		}
		tVectorInsert.Stop();

		ClearCache();
		stdVectorInsert.Start();
		for (size_t i = 0; i < countToAdd; i++) {
			const size_t num = rand() % 16;
			const size_t pos = rand() % ideal.size();

			ideal.insert(ideal.begin() + pos, 1u, test_type((uint32_t) i));
		}
		stdVectorInsert.Stop();
	}
	std::cout << std::endl;

	std::cout << "Performance test add/emplace:\n\tstd::vector " << stdVector.Result() << " ms"
			  << "\n\tarray " << tVector.Result() << " ms" << std::endl;
	std::cout << "Performance test RemoveAtSwap:\n\tstd::vector " << stdVectorRemoveSwap.Result()
			  << " ms"
			  << "\n\tarray " << tVectorRemoveSwap.Result() << " ms" << std::endl;
	std::cout << "Performance test RemoveAt:\n\tstd::vector " << stdVectorRemoveAt.Result() << " ms"
			  << "\n\tarray " << tVectorRemoveAt.Result() << " ms" << std::endl;
	std::cout << "Performance test Insert:\n\tstd::vector " << stdVectorInsert.Result() << " ms"
			  << "\n\tarray " << tVectorInsert.Result() << " ms" << std::endl;
	std::cout << "Performance test Sort:\n\tstd::vector " << stdVectorSort.Result() << " ms"
			  << "\n\tarray " << tVectorSort.Result() << " ms" << std::endl;
}

s32 array_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<s32>(10000);
	Passed = Passed && SanityCheck<complex_type>(10000);
	Passed = Passed && SanityCheck<complex_type_realloc>(10000);
	return Passed ? 0 : 1;
}

void array_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	PerformanceTests<bytes_struct<24>>(10000, 1000);
	PerformanceTests<complex_type>(10000, 1000);
	PerformanceTests<complex_type_realloc>(10000, 1000);
}

TEST_ENTRY(array_test);
