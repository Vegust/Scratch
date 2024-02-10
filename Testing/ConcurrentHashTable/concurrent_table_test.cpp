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

bool Compare(std::unordered_map<s32, s32>& Ideal, concurrent_hash_table<s32, s32>& Test) {
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

void IdealThreadInsertions(std::unordered_map<s32, s32>* Ideal, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = 0; i < Count; ++i) {
		int RandomKey = Distribution(Generator);
		Ideal->operator[](RandomKey) = RandomKey;
	}
}

void TestThreadInsertions(concurrent_hash_table<s32, s32>* Test, int Seed, int Count, int Min, int Max) {
	std::default_random_engine Generator(Seed);
	std::uniform_int_distribution<int> Distribution(Min, Max);
	for (int i = 0; i < Count; ++i) {
		int RandomKey = Distribution(Generator);
		Test->operator[](RandomKey) = RandomKey;
	}
}

template <typename key_type, typename value_type>
static bool SanityCheck() {
	int Count = 10000;
	int NumThreads = 10;	// std::thread::hardware_concurrency();
	std::vector<int> Seeds;
	std::vector<std::pair<int, int>> KeyRanges;
	Seeds.reserve(NumThreads);
	KeyRanges.reserve(NumThreads);
	for (int i = 0; i < NumThreads; ++i) {
		Seeds.push_back(i);
		// This makes values different in sequential vs non-sequential unless they are the same as keys or something
		KeyRanges.push_back({0, 10000000});
		// KeyRanges.push_back({i * Count, (i + 1) * Count - 1});
	}
	std::unordered_map<s32, s32> SequentialIdeal;
	concurrent_hash_table<s32, s32> SequentialTest;
	concurrent_hash_table<s32, s32> MultithreadingTest;

	timer SequentialInsertIdealTimer;
	timer SequentialInsertTestTimer;
	timer MultithreadingInsertTestTimer;

	SequentialInsertIdealTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		IdealThreadInsertions(&SequentialIdeal, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialInsertIdealTimer.Stop();

	SequentialInsertTestTimer.Start();
	for (int i = 0; i < NumThreads; ++i) {
		TestThreadInsertions(&SequentialTest, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second);
	}
	SequentialInsertTestTimer.Stop();

	bool SequentialInsertResult = Compare(SequentialIdeal, SequentialTest);

	std::cout << "SEQUENTIAL INSERT IDEAL TIME: " << SequentialInsertIdealTimer.Result() << "ms" << std::endl;
	std::cout << "SEQUENTIAL INSERT TEST  TIME: " << SequentialInsertTestTimer.Result() << "ms" << std::endl;
	std::cout << "SEQUENTIAL INSERT CHECK " << SequentialInsertResult << std::endl;

	MultithreadingInsertTestTimer.Start();
	std::vector<std::thread> Threads{};
	for (int i = 0; i < NumThreads; ++i) {
		Threads.push_back(std::thread{
			TestThreadInsertions, &MultithreadingTest, Seeds[i], Count, KeyRanges[i].first, KeyRanges[i].second});
	}
	for (int i = 0; i < NumThreads; ++i) {
		Threads[i].join();
	}
	MultithreadingInsertTestTimer.Stop();

	bool MultithreadingInsertResult = Compare(SequentialIdeal, MultithreadingTest);

	std::cout << "MULTITHREAD INSERT TEST TIME: " << MultithreadingInsertTestTimer.Result() << "ms" << std::endl;
	std::cout << "MULTITHREAD INSERT CHECK " << MultithreadingInsertResult << std::endl;

	//	std::atomic<s32> Counter;
	//	auto Lambda = [&Counter](s64 TestInput) {
	//		std::osyncstream syncStream{std::cout};
	//		++Counter;
	//		syncStream << std::this_thread::get_id() << "\n";
	//	};
	//	std::vector<std::thread> Threads{};
	//	for (int i = 0; i < NumThreads; ++i) {
	//		Threads.push_back(std::thread{Lambda, Count});
	//	}
	//	for (int i = 0; i < NumThreads; ++i) {
	//		Threads[i].join();
	//	}
	//	std::cout << Counter << std::endl;
	return true;
}

s32 concurrent_table_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck<complex_type, complex_type>();
	return Passed ? 0 : 1;
}

void concurrent_table_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
}

TEST_ENTRY(concurrent_table_test);
