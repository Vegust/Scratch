#include "../testing_shared.h"
#include "String/str_conversions.h"
#include "Logs/logs.h"
#include "Time/timestamp.h"

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
	{
		str TestShort{"123"};
		str TestLong{"123456789101112131415161718192021222324252262728293031323334353637383940"};

		for (s32 Index = 0; Index < Count; Index++) {
			const s32 Number = (rand() % 10000000) - 5000000;
			std::string Ideal = std::to_string(Number);
			str Tested = strings::ToString(Number);
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
	}
	TEST_CHECK(Valid, "integer to str");

	{
		const str TestCase1{" 100 dfd"};
		if (auto Result = strings::GetNumber<s32>(TestCase1)) {
			Valid = 100 == Result.GetValue();
			CHECK(Valid);
		} else {
			Valid = false;
			CHECK(Valid);
		}

		const str TestCase2{"   -123123 dfdf"};
		if (auto Result = strings::GetNumber<u32>(TestCase2)) {
			Valid = false;
			CHECK(Valid);
		} else {
			Valid = Valid && true;
			CHECK(Valid);
		}

		const str TestCase3{"209370217302173270273082738270312703817203712037092173021730921730192730219"};
		if (auto Result = strings::GetNumber<u32>(TestCase3)) {
			Valid = false;
			CHECK(false);
		} else {
			Valid = Valid && true;
			CHECK(Valid);
		}

		for (s32 Index = 0; Index < Count; Index++) {
			const s32 Number = (rand() % 10000000) - 5000000;
			const s32 RoundTrip = strings::GetNumberChecked<s32>(strings::ToString(Number));
			if (Number != RoundTrip) {
				Valid = false;
				CHECK(Valid);
				break;
			}
		}
	}
	TEST_CHECK(Valid, "str to integer");

	{
		float Problem{-1};
		auto ProblemResult = strings::ToString(Problem);

		float TestCase1{INFINITY};
		Valid = "inf" == strings::ToString(TestCase1);
		CHECK(Valid);

		float TestCase2{-INFINITY};
		Valid = Valid && ("-inf" == strings::ToString(TestCase2));
		CHECK(Valid);

		float TestCase3{NAN};
		Valid = Valid && ("NaN" == strings::ToString(TestCase3));
		CHECK(Valid);

		str TestCase4{"10 10 10 10 10"};
		if (auto Result = strings::GetNumber<double>(TestCase4)) {
			Valid = Valid && (Result.GetValue() == 10);
			CHECK(Valid);
		} else {
			Valid = false;
			CHECK(Valid);
		}

		str TestCase5{"fgdghjdfhgjdhfjghfj"};
		if (auto Result = strings::GetNumber<double>(TestCase5)) {
			Valid = false;
			CHECK(Valid);
		} else {
			CHECK(Valid);
		}

		for (s32 Index = 0; Index < Count; Index++) {
			const s32 Sign = (rand() % 2) ? (1) : (-1);
			const float Number = Sign * ((float) rand() / (float) rand());
			const float RoundTrip = strings::GetNumberChecked<float>(strings::ToString(Number));
			if (Number != RoundTrip) {
				Valid = false;
				CHECK(Valid);
				break;
			}
		}
	}
	TEST_CHECK(Valid, "float to str and str to float");

	str Test = strings::Format("Testing formatting: {} {} {} {}", &Valid, "test1", -0.1f);

	atom TestCategory{"Test Log Category"};

	logs::Log<logs::verbosity::info>(TestCategory, "Testing formatting: {} {} {} {}", &Valid, "test1", -0.1f);
	logs::Log<logs::verbosity::warning>(TestCategory, "Testing formatting: {} {} {} {}", &Valid, "test1", -0.1f);
	logs::Log<logs::verbosity::debug>(TestCategory, "Testing formatting: {} {} {} {}", &Valid, "test1", -0.1f);
	logs::Log<logs::verbosity::error>(TestCategory, "Testing formatting: {} {} {} {}", &Valid, "test1", -0.1f);

	str Pointer1 = strings::ToString(nullptr);
	str Pointer2 = strings::ToString(&Test);

	timestamp TimeTest = timestamp::GetCurrentUTC();
	logs::Log(
		"Year: {}, Month: {}, Day {}, Hour {}, Minute {}, Second {}",
		TimeTest.GetYear(),
		TimeTest.GetMonth(),
		TimeTest.GetDay(),
		TimeTest.GetHour(),
		TimeTest.GetMinute(),
		TimeTest.GetSecond());

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