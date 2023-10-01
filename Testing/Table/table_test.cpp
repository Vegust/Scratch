#include "../testing_shared.h"
#include "Containers/hash_table.h"

#include <random>
#include <unordered_map>

struct table_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename key_type, typename test_type>
static bool SanityCheck(s64 Count) {
	std::cout << "------------------------------------------" << std::endl;
	const std::string InfoString1 = std::string("Testing with key ") + typeid(key_type).name() + ", " +
								   std::to_string(sizeof(key_type)) + " bytes";
	const std::string InfoString2 = std::string("Testing with value ") + typeid(test_type).name() + ", " +
								   std::to_string(sizeof(test_type)) + " bytes";
	std::cout << InfoString1 << std::endl << InfoString2 << std::endl;
	
	
	if constexpr (requires { test_type::NumInstances; }) {
		test_type::NumInstances = 0;
	}

	hash_table<key_type, test_type> container;
	std::unordered_map<key_type, test_type> ideal;

	for (size_t i = 0; i < Count; i++) {
		ideal[MakeValue<key_type>(i)] = MakeValue<test_type>(i * 3);
		container[MakeValue<key_type>(i)] = MakeValue<test_type>(i * 3);
	}

	bool Valid = true;
	for (size_t i = 0; i < Count / 2; i++) {
		ideal.erase(i * 2);
		container.RemoveOne(i * 2);

		for (const auto& el : ideal) {
			if (container[el.first] != el.second) {
				Valid = false;
				break;
			}
		}

		for (const auto& el : container) {
			if (ideal[el.mKey] != el.mValue) {
				Valid = false;
				break;
			}
		}
	}

	TEST_CHECK(Valid, "insert and remove");

	container.Clear();
	ideal.clear();
	if constexpr (requires { test_type::NumInstances; }) {
		TEST_CHECK(test_type::NumInstances == 0, "object construction/destruction");
	}
	return true;
}

template <typename TKeyType, typename TValueType>
class TestCase_MapPerfromance {
public:
	static void RunTests(u64 Size, u64 Iterations, bool DoColdCache) {
		const std::string tMapClassName =
			std::string("Testing with key=(") + typeid(TKeyType).name() + "), value=(" +
			typeid(TValueType).name() + "), " + std::to_string(sizeof(TValueType)) + " bytes";
		printf("%s\n", tMapClassName.c_str());
		PerformanceTests(Size, Iterations, DoColdCache);
		printf("\n");
	}

	struct timer_tuple {
		timer Std{};
		timer Table{};
	};

	static std::string TimerResults(const timer_tuple& Timers) {
		return "std::map\t" + std::to_string(Timers.Std.Result()) + " ms \n\t" + "hash_table\t" +
			   std::to_string(Timers.Table.Result()) + " ms \n";
	}

	FORCEINLINE static TValueType MakeValue(size_t index) {
		return index * 3;
	}

	FORCEINLINE static TKeyType MakeKey(size_t index, size_t Size, size_t Seed) {
		return (index + Seed) % Size;
	}

	FORCEINLINE static TValueType MakeValue(size_t index)
		requires(std::is_same<TValueType, std::string>::value)
	{
		return std::to_string(index * 3);
	}

	FORCEINLINE static TKeyType MakeKey(size_t index, size_t Size, size_t Seed)
		requires(std::is_same<TKeyType, std::string>::value)
	{
		index = index + Seed;
		return std::to_string((index + Seed) % Size);
	}

	static void PerformanceTests(u64 Size, u64 Iterations, bool DoColdCache) {
		std::cout << "Size = " << Size << ", " << Iterations << " iterations." << std::endl;

		timer_tuple Timers[7]{};

		volatile float misses = 0;	  // just to prevent optimizing away calculations

		for (int iter = 0; iter < Iterations; ++iter) {
			const s32 seed = rand();

			hash_table<TKeyType, TValueType> table;
			std::unordered_map<TKeyType, TValueType> standard;

			ClearCache();

			// -------------- operator[] ----------------
			for (int type : {0, 1}) {
				switch ((type + iter) % 2) {
					case 0:
						Timers[0].Std.Start();
						for (size_t i = 0; i < Size; i++) {
							standard[MakeKey(i, Size, seed)] = MakeValue(i * 3);
						}
						Timers[0].Std.Stop();
						break;
					case 1:
						Timers[0].Table.Start();
						for (size_t i = 0; i < Size; i++) {
							table[MakeKey(i, Size, seed)] = MakeValue(i * 3);
						}
						Timers[0].Table.Stop();
						break;
				}
			}
			// ------------------------------------------

			ClearCache();

			// ---------- operator[] reserved -----------
			table.Clear();
			standard.clear();
			for (int type : {0, 1}) {
				switch ((type + iter) % 2) {
					case 0:
						Timers[1].Std.Start();
						standard.reserve(Size);
						for (size_t i = 0; i < Size; i++) {
							standard[MakeKey(i, Size, seed)] = MakeValue(i * 3);
						}
						Timers[1].Std.Stop();
						break;
					case 1:
						Timers[1].Table.Start();
						table.EnsureCapacity(Size);
						for (size_t i = 0; i < Size; i++) {
							table[MakeKey(i, Size, seed)] = MakeValue(i * 3);
						}
						Timers[1].Table.Stop();
						break;
				}
			}
			// ------------------------------------------

			ClearCache();

			// -------- ContainsKey 0.5 misses ---------
			{
				std::random_device rd;
				std::mt19937 g(rd());

				for (int type : {0, 1}) {
					switch ((type + iter) % 2) {
						case 0:
							g.seed(seed);
							Timers[2].Std.Start();
							for (size_t i = 0; i < Size; i++) {
								const size_t value = i % 2 ? g() : g() % Size;
								if (standard.find(MakeKey(value, Size, seed)) == standard.end()) {
									misses = misses + 1;
								}
							}
							Timers[2].Std.Stop();
							break;
						case 1:
							g.seed(seed);
							Timers[2].Table.Start();
							for (size_t i = 0; i < Size; i++) {
								const size_t value = i % 2 ? g() : g() % Size;
								if (!table.Contains(MakeKey(value, Size, seed))) {
									misses = misses + 1;
								}
							}
							Timers[2].Table.Stop();
							break;
					}
				}
			}
			// ------------------------------------------

			ClearCache();

			// -------- ContainsKey 0.0 misses ---------
			{
				std::random_device rd;
				std::mt19937 g(rd());

				for (int type : {0, 1}) {
					switch ((type + iter) % 2) {
						case 0:
							g.seed(seed);
							Timers[3].Std.Start();
							for (size_t i = 0; i < Size; i++) {
								const size_t value = g() % Size;
								if (standard.find(MakeKey(value, Size, seed)) == standard.end()) {
									misses = misses + 1;
								}
							}
							Timers[3].Std.Stop();
							break;
						case 1:
							g.seed(seed);
							Timers[3].Table.Start();
							for (size_t i = 0; i < Size; i++) {
								const size_t value = g() % Size;
								if (!table.Contains(MakeKey(value, Size, seed))) {
									misses = misses + 1;
								}
							}
							Timers[3].Table.Stop();
							break;
					}
				}
			}
			// ------------------------------------------

			ClearCache();

			if (DoColdCache) {
				// ---- ColdCache ContainsKey 0.5 misses ----
				{
					std::random_device rd;
					std::mt19937 g(rd());

					for (int type : {0, 1}) {
						switch ((type + iter) % 2) {
							case 0: {
								timer CacheOffset;
								g.seed(seed);
								Timers[4].Std.Start();
								for (size_t i = 0; i < Size; i++) {
									CacheOffset.Start();
									ClearCache();
									CacheOffset.Stop();
									const size_t value = i % 2 ? g() : g() % Size;
									if (standard.find(MakeKey(value, Size, seed)) ==
										standard.end()) {
										misses = misses + 1;
									}
								}
								Timers[4].Std.Stop();
								Timers[4].Std.Offset(CacheOffset);
								break;
							}
							case 1: {
								timer CacheOffset;
								g.seed(seed);
								Timers[4].Table.Start();
								for (size_t i = 0; i < Size; i++) {
									CacheOffset.Start();
									ClearCache();
									CacheOffset.Stop();
									const size_t value = i % 2 ? g() : g() % Size;
									if (!table.Contains(MakeKey(value, Size, seed))) {
										misses = misses + 1;
									}
								}
								Timers[4].Table.Stop();
								Timers[4].Table.Offset(CacheOffset);
								break;
							}
						}
					}
				}
				// ------------------------------------------

				ClearCache();

				// ---- ColdCache ContainsKey 0.0 misses ----
				{
					std::random_device rd;
					std::mt19937 g(rd());

					for (int type : {0, 1}) {
						switch ((type + iter) % 2) {
							case 0: {
								timer CacheOffset;
								g.seed(seed);
								Timers[5].Std.Start();
								for (size_t i = 0; i < Size; i++) {
									CacheOffset.Start();
									ClearCache();
									CacheOffset.Stop();
									const size_t value = g() % Size;
									if (standard.find(MakeKey(value, Size, seed)) ==
										standard.end()) {
										misses = misses + 1;
									}
								}
								Timers[5].Std.Stop();
								Timers[5].Std.Offset(CacheOffset);
								break;
							}
							case 1: {
								timer CacheOffset;
								g.seed(seed);
								Timers[5].Table.Start();
								for (size_t i = 0; i < Size; i++) {
									CacheOffset.Start();
									ClearCache();
									CacheOffset.Stop();
									const size_t value = g() % Size;
									if (!table.Contains(MakeKey(value, Size, seed))) {
										misses = misses + 1;
									}
								}
								Timers[5].Table.Stop();
								Timers[5].Table.Offset(CacheOffset);
								break;
							}
						}
					}
				}
			}
			// ------------------------------------------

			ClearCache();

			// --------------- Remove -------------------

			for (int type : {0, 1}) {
				switch ((type + iter) % 2) {
					case 0:
						Timers[6].Std.Start();
						for (size_t i = 0; i < Size; i++) {
							if (i % 2) {
								standard.erase(MakeKey(i, Size, seed));
							}
						}
						Timers[6].Std.Stop();
						break;
					case 1:
						Timers[6].Table.Start();
						for (size_t i = 0; i < Size; i++) {
							if (i % 2) {
								table.RemoveOne(MakeKey(i, Size, seed));
							}
						}
						Timers[6].Table.Stop();
						break;
				}
			}
			// ------------------------------------------
		}

		std::cout << "Performance test operator[]:\n\t" << TimerResults(Timers[0]);
		std::cout << "Performance test operator[] with reserved space:\n\t"
				  << TimerResults(Timers[1]);
		std::cout << "Performance test ContainsKey (50% misses):\n\t" << TimerResults(Timers[2]);
		std::cout << "Performance test ContainsKey (0% misses):\n\t" << TimerResults(Timers[3]);
		if (DoColdCache) {
			std::cout << "Performance test ContainsKey with cold cache (50% misses):\n\t"
					  << TimerResults(Timers[4]);
			std::cout << "Performance test ContainsKey with cold cache (0% misses):\n\t"
					  << TimerResults(Timers[5]);
		}
		std::cout << "Performance test Remove:\n\t" << TimerResults(Timers[6]);
	}
};

s32 table_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<complex_type,complex_type>(10000);
	Passed = Passed && SanityCheck<complex_type_realloc, complex_type_realloc>(10000);
	return Passed ? 0 : 1;
}

void table_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	printf("\nStarting Map benchmark...\n");

	bool ColdCacheTest = false;
	if (ColdCacheTest) {
		// Cold cache setup (invalidating caches after each lookup) is super slow so tests are
		// smaller

		// no big differences with cold cache, everything is just ~10 time slower without
		// relative changes

		TestCase_MapPerfromance<size_t, size_t>::RunTests(50000, 4, true);
		TestCase_MapPerfromance<size_t, std::string>::RunTests(50000, 4, true);
		TestCase_MapPerfromance<size_t, bytes_struct<64>>::RunTests(50000, 4, true);
		TestCase_MapPerfromance<size_t, bytes_struct<256>>::RunTests(50000, 4, true);
		TestCase_MapPerfromance<size_t, bytes_struct<512>>::RunTests(50000, 4, true);
	} else {
		constexpr u64 Count = 500000;
		TestCase_MapPerfromance<size_t, size_t>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<size_t, std::string>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<size_t, bytes_struct<512>>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<size_t, bytes_struct<1024>>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<size_t, bytes_struct<2048>>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<std::string, size_t>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<std::string, std::string>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<std::string, bytes_struct<512>>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<std::string, bytes_struct<1024>>::RunTests(Count, 10, false);
		TestCase_MapPerfromance<std::string, bytes_struct<2048>>::RunTests(Count, 10, false);
	}
}

TEST_ENTRY(table_test);
