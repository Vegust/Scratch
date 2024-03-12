#include "../testing_shared.h"
#include "Asset/Json/json_serialization.h"

struct example_type {
	float Percentage;
	s32 Number;

	static span<reflection::field> GetFields() {
		using namespace reflection;
		static array<field, 2> Fields{
			field{"Percentage", GetFieldType<float>(), offsetof(example_type, Percentage)},
			field{	  "Number",	GetFieldType<s32>(), offsetof(example_type,		Number)}
		};
		return Fields;
	}
};

struct array_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

s32 array_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	return Passed ? 0 : 1;
}

void array_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
}

TEST_ENTRY(array_test);