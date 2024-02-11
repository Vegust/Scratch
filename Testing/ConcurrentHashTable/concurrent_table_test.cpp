#include "Concurrency/concurrent_hash_table.h"
#include "../testing_shared.h"

#include <thread>
#include <syncstream>
#include <unordered_map>
#include <random>

struct concurrent_table_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

template <typename key_type, typename value_type>
bool Compare(std::unordered_map<key_type, value_type>& Ideal, concurrent_hash_table<key_type, value_type>& Test) {
	if (Ideal.size() != Test.GetSize()) {
		return false;
	}
	for (auto& Element : Ideal) {
		if (!Test.ContainsKey(Element.first) || Test[Element.first] != Element.second) {
			return false;
		}
	}
	for (auto& Element : Test) {
		if (!Ideal.contains(Element.Key) || Ideal[Element.Key] != Element.Value) {
			return false;
		}
	}
	return true;
}

template <typename key_type, typename value_type>
void IdealThreadInsertions(std::unordered_map<key_type, value_type>* Ideal, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		int RandomValue = i;//Distribution(Generator);
		Ideal->operator[](RandomKey) = RandomValue;
	}
}

template <typename key_type, typename value_type>
void TestThreadInsertions(concurrent_hash_table<key_type, value_type>* Test, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		int RandomValue = i;//Distribution(Generator);
		Test->operator[](RandomKey) = RandomValue;
	}
}

template <typename key_type, typename value_type>
void IdealThreadDeletions(std::unordered_map<key_type, value_type>* Ideal, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		Ideal->erase(RandomKey);
	}
}

template <typename key_type, typename value_type>
void TestThreadDeletions(concurrent_hash_table<key_type, value_type>* Test, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		Test->Remove(RandomKey);
	}
}

template <typename key_type, typename value_type>
void IdealThreadMix(std::unordered_map<key_type, value_type>* Ideal, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	std::uniform_int_distribution<int> SwitchDistribution(0, 1);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		if (SwitchDistribution(Generator)) {
			Ideal->erase(RandomKey);
		} else {
			int RandomValue = i;//Distribution(Generator);
			Ideal->operator[](RandomKey) = RandomValue;
		}
	}
}

template <typename key_type, typename value_type>
void TestThreadMix(concurrent_hash_table<key_type, value_type>* Test, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	std::uniform_int_distribution<int> SwitchDistribution(0, 1);
	for (int i = Min; i < Min + Count; ++i) {
		int RandomKey = i;//Distribution(Generator);
		if (SwitchDistribution(Generator)) {
			Test->Remove(RandomKey);
		} else {
			int RandomValue = i;//Distribution(Generator);
			Test->operator[](RandomKey) = RandomValue;
		}
	}
}

template <typename key_type, typename value_type>
static bool SanityCheck(int Count, int NumThreads, bool SameKeys) {
	std::vector<int> Seeds;
	std::vector<std::pair<int, int>> KeyRanges;
	Seeds.reserve(NumThreads);
	KeyRanges.reserve(NumThreads);
	for (int i = 0; i < NumThreads; ++i) {
		Seeds.push_back(i);
		if (SameKeys) {
			KeyRanges.push_back({0, Count * NumThreads});
		} else {
			KeyRanges.push_back({i * Count, (i + 1) * Count - 1});
		}
	}
	std::unordered_map<key_type, value_type> SequentialIdeal;
	concurrent_hash_table<key_type, value_type> SequentialTest;
	concurrent_hash_table<key_type, value_type> MultithreadingTest;

	timer SequentialInsertIdealTimer;
	timer SequentialInsertTestTimer;
	timer MultithreadingInsertTestTimer;

	SequentialInsertIdealTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		IdealThreadInsertions<key_type, value_type>(
			&SequentialIdeal, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialInsertIdealTimer.Stop();

	SequentialInsertTestTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		TestThreadInsertions<key_type, value_type>(
			&SequentialTest, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialInsertTestTimer.Stop();

	bool SequentialInsertResult = Compare(SequentialIdeal, SequentialTest);

	std::cout << "SEQUENTIAL INSERT IDEAL TIME: " << SequentialInsertIdealTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL INSERT TEST  TIME: " << SequentialInsertTestTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL INSERT CHECK " << SequentialInsertResult << std::endl;

	MultithreadingInsertTestTimer.Start();
	std::vector<std::thread> Threads{};
	for (int i = 0; i < NumThreads; ++i) {
		Threads.push_back(std::thread{
			TestThreadInsertions<key_type, value_type>,
			&MultithreadingTest,
			Seeds[i],
			Count,
			KeyRanges[i].first,
			KeyRanges[i].second});
	}
	for (int i = 0; i < NumThreads; ++i) {
		Threads[i].join();
	}
	MultithreadingInsertTestTimer.Stop();

	bool MultithreadingInsertResult = Compare(SequentialIdeal, MultithreadingTest);

	std::cout << "MLTITHREAD INSERT TEST  TIME: " << MultithreadingInsertTestTimer.Result() << " ms" << std::endl;
	std::cout << "MLTITHREAD INSERT CHECK " << MultithreadingInsertResult << std::endl;
	CHECK(MultithreadingInsertResult);

	// ---------------------------------------------------------------------------

	timer SequentialMixIdealTimer;
	timer SequentialMixTestTimer;
	timer MultithreadingMixTestTimer;

	SequentialMixIdealTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		IdealThreadMix<key_type, value_type>(
			&SequentialIdeal, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialMixIdealTimer.Stop();

	SequentialMixTestTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		TestThreadMix<key_type, value_type>(&SequentialTest, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialMixTestTimer.Stop();

	bool SequentialMixResult = Compare(SequentialIdeal, SequentialTest);

	std::cout << "SEQUENTIAL MIX IDEAL TIME: " << SequentialMixIdealTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL MIX TEST  TIME: " << SequentialMixTestTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL MIX CHECK " << SequentialMixResult << std::endl;

	MultithreadingMixTestTimer.Start();
	std::vector<std::thread> Threads3{};
	for (int i = 0; i < NumThreads; ++i) {
		Threads3.push_back(std::thread{
			TestThreadMix<key_type, value_type>,
			&MultithreadingTest,
			Seeds[i],
			Count,
			KeyRanges[i].first,
			KeyRanges[i].second});
	}
	for (int i = 0; i < NumThreads; ++i) {
		Threads3[i].join();
	}
	MultithreadingMixTestTimer.Stop();

	bool MultithreadingMixResult = Compare(SequentialIdeal, MultithreadingTest);

	std::cout << "MLTITHREAD MIX TEST  TIME: " << MultithreadingMixTestTimer.Result() << " ms" << std::endl;
	std::cout << "MLTITHREAD MIX CHECK " << MultithreadingMixResult << std::endl;
	CHECK(MultithreadingMixResult);

	// ---------------------------------------------------------------------------

	timer SequentialDeleteIdealTimer;
	timer SequentialDeleteTestTimer;
	timer MultithreadingDeleteTestTimer;

	SequentialDeleteIdealTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		IdealThreadDeletions<key_type, value_type>(
			&SequentialIdeal, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialDeleteIdealTimer.Stop();

	SequentialDeleteTestTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		TestThreadDeletions<key_type, value_type>(
			&SequentialTest, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialDeleteTestTimer.Stop();

	bool SequentialDeleteResult = Compare(SequentialIdeal, SequentialTest);

	std::cout << "SEQUENTIAL DELETE IDEAL TIME: " << SequentialDeleteIdealTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL DELETE TEST  TIME: " << SequentialDeleteTestTimer.Result() << " ms" << std::endl;
	std::cout << "SEQUENTIAL DELETE CHECK " << SequentialDeleteResult << std::endl;

	MultithreadingDeleteTestTimer.Start();
	std::vector<std::thread> Threads2{};
	for (int i = 0; i < NumThreads; ++i) {
		Threads2.push_back(std::thread{
			TestThreadDeletions<key_type, value_type>,
			&MultithreadingTest,
			Seeds[i],
			Count,
			KeyRanges[i].first,
			KeyRanges[i].second});
	}
	for (int i = 0; i < NumThreads; ++i) {
		Threads2[i].join();
	}
	MultithreadingDeleteTestTimer.Stop();

	bool MultithreadingDeleteResult = Compare(SequentialIdeal, MultithreadingTest);

	std::cout << "MLTITHREAD DELETE TEST  TIME: " << MultithreadingDeleteTestTimer.Result() << " ms" << std::endl;
	std::cout << "MLTITHREAD DELETE CHECK " << MultithreadingDeleteResult << std::endl;
	CHECK(MultithreadingDeleteResult);

	return true;
}

s32 concurrent_table_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<s64, bytes_struct<2048>>(50000, 16, false);
	return Passed ? 0 : 1;
}

void concurrent_table_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
}

TEST_ENTRY(concurrent_table_test);
