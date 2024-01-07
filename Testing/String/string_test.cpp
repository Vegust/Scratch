#include "../testing_shared.h"
#include "String/str_util.h"
#include "String/atom.h"

struct set_test {
	s32 Test(const std::span<char*>& Args);
	void Benchmark(const std::span<char*>& Args);
};

static bool SanityCheck(s64 Count) {
	std::cout << "------------------------------------------" << std::endl;
	//	const std::string InfoString = std::string("Testing with ") + typeid(test_type).name() + ", " +
	//								   std::to_string(sizeof(test_type)) + " bytes";
	//	std::cout << InfoString << std::endl;

	bool Valid = true;

	//	std::cout << sizeof(str_view) << std::endl;
	//	std::cout << sizeof(str) << std::endl;
	//	std::cout << sizeof(dyn_array<u32>) << std::endl;
	//	std::cout << sizeof(std::vector<u32>) << std::endl;
	//	std::cout << sizeof(std::string) << std::endl;

	str TestShort{"123"};
	str TestLong{"123456789101112131415161718192021222324252262728293031323334353637383940"};

	for (s32 Index = 0; Index < Count; Index++) {
		const s32 Number = (rand() % 10000000) - 5000000;
		std::string Ideal = std::to_string(Number);
		str Tested = str_util::FromInt(Number);
		if (Ideal.length() != Tested.GetByteLength()) {
			Valid = false;
			CHECK(Valid);
			break;
		}
		for (s32 ByteIndex = 0; ByteIndex < Ideal.length(); ByteIndex++) {
			if (Ideal.at(ByteIndex) != Tested.GetAt(ByteIndex)) {
				Valid = false;
				CHECK(Valid);
				break;
			}
		}
	}
	TEST_CHECK(Valid, "integer to str");

	const str TestCase1{" 100 dfd"};
	if (auto Result = str_util::GetNumber<s32>(TestCase1)) {
		Valid = 100 == Result.GetValue();
		CHECK(Valid);
	} else {
		Valid = false;
		CHECK(Valid);
	}
	
	const str TestCase2{"   -123123 dfdf"};
	if (auto Result = str_util::GetNumber<u32>(TestCase2)) {
		Valid = false;
		CHECK(Valid);
	} else {
		Valid = true;
		CHECK(Valid);
	}
	
	const str TestCase3{"209370217302173270273082738270312703817203712037092173021730921730192730219"};
	if (auto Result = str_util::GetNumber<u32>(TestCase3)) {
		Valid = false;
		CHECK(false);
	} else {
		Valid = true;
		CHECK(Valid);
	}
	
	for (s32 Index = 0; Index < Count; Index++) {
		const s32 Number = (rand() % 10000000) - 5000000;
		const s32 RoundTrip = str_util::GetNumberChecked<s32>(str_util::FromInt(Number));
		if (Number != RoundTrip) {
			Valid = false;
			CHECK(Valid);
			break;
		}
	}
	TEST_CHECK(Valid, "str to integer");
	
	float TestCaseFloat1{-0.000000112345};
	str TestCaseResult1 = str_util::FromFloat(TestCaseFloat1);
	
	TEST_CHECK(Valid, "float to str to float");

	return true;
}

s32 set_test::Test(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	bool Passed = true;
	Passed = Passed && SanityCheck(10000);
	return Passed ? 0 : 1;
}

void set_test::Benchmark(const std::span<char*>& Args) {
	TEST_PRINT_LINE();
	std::cout << "no benchmarking for string" << std::endl;
}

TEST_ENTRY(set_test)