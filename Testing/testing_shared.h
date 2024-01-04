#pragma once

#include "core.h"
#include "Core/Utility/hash.h"

#include <chrono>
#include <iostream>
#include <span>

#ifdef _WIN32
#include <Windows.h>
#include "psapi.h"

size_t GetTotalUsedVirtualMemory() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*) &pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG virtualMemUsed = memInfo.ullTotalVirtual - memInfo.ullAvailVirtual;
	return (size_t) virtualMemUsedByMe;
}
#endif

// NOTE: commented out since not used and increases GCC ocmpilation times by a lot
//constexpr u64 GarbageLength = 20 * 1024 * 1024;
//static u64 GarbageBytes[GarbageLength]{0xff};

FORCEINLINE void ClearCache() {
//	for (int i = 0; i < 2000; i++) {
//		GarbageBytes[rand() % GarbageLength] = GarbageBytes[rand() % GarbageLength];
//	}
}

struct timer {
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
	time_point mStart{};
	float mAccumulator{0};

	void Start() {
		mStart = std::chrono::high_resolution_clock::now();
	}

	void Stop() {
		time_point End = std::chrono::high_resolution_clock::now();
		s64 Time = std::chrono::duration_cast<std::chrono::microseconds>(End - mStart).count();
		mAccumulator += Time / 1000.f;
	}

	[[nodiscard]] float Result() const {
		return mAccumulator;
	}

	void Offset(const timer& OffsetTimer) {
		mAccumulator -= OffsetTimer.mAccumulator;
	}

	void Clear() {
		mStart = {};
		mAccumulator = 0;
	}
};

template <bool Relocatable = false>
struct complex_type_template {
	static inline s64 NumInstances = 0;
	
	static constexpr bool MemcopyRelocatable = Relocatable;

	complex_type_template() : complex_type_template(0) {
	}

	complex_type_template(u8 value) {
		NumInstances++;
		m_value = value;
		m_data = new u64[8];
		memset(m_data, value, 8 * sizeof(u64));
	}

	complex_type_template(const complex_type_template& rhs) {
		NumInstances++;
		m_data = new u64[8];
		m_value = rhs.m_value;
		memcpy(m_data, rhs.m_data, 8 * sizeof(u64));
	}

	~complex_type_template() {
		NumInstances--;
		delete[] m_data;
	}

	bool operator==(const complex_type_template& rhs) const {
		return m_value == rhs.m_value && memcmp(m_data, rhs.m_data, 8 * sizeof(u64)) == 0;
	}

	bool operator<(const complex_type_template& rhs) const {
		return m_value < rhs.m_value;
	}

	complex_type_template& operator=(const complex_type_template& rhs) {
		if (&rhs == this) {
			return *this;
		}
		m_value = rhs.m_value;
		memcpy(m_data, rhs.m_data, 8 * sizeof(u64));
		return *this;
	}

	[[nodiscard]] hash::hash_type GetHash() const {
		return hash::Hash(m_value);
	}

	u64* m_data = nullptr;
	u8 m_value;
};

using complex_type = complex_type_template<false>;
using complex_type_realloc = complex_type_template<true>;

template <>
struct std::hash<complex_type> {
	std::size_t operator()(const complex_type& Data) const noexcept {
		return std::hash<u8>{}(Data.m_value);
	}
};

template <>
struct std::hash<complex_type_realloc> {
	std::size_t operator()(const complex_type_realloc& Data) const noexcept {
		return std::hash<u8>{}(Data.m_value);
	}
};

#pragma pack(push, 1)

#if defined(__GNUC__)	 // GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

template <int Size>
struct bytes_struct {
	bytes_struct() {
		memset(&m_test, 0, Size);
	}

	bytes_struct(uint32_t value) {
		memset(&m_test, value, Size);
	}

	bool operator==(const bytes_struct& rhs) const {
		return m_test[0] == rhs.m_test[0];
	}

	bool operator<(const bytes_struct& rhs) const {
		return m_test[0] < rhs.m_test[0];
	}

	~bytes_struct() {
	}

	u8 m_test[Size];
};
#if defined(__GNUC__)	 // GCC
#pragma GCC diagnostic pop
#endif
#pragma pack(pop)

template <typename value_type>
FORCEINLINE static value_type MakeValue(s64 value) {
	if constexpr (std::is_same<value_type, std::string>::value == true) {
		return std::to_string(value);
	} else {
		return value_type(value);
	}
}

#define TEST_PRINT_LINE() \
	{ std::cout << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl; }

#define TEST_CHECK(Expression, TestName)                                              \
	{                                                                                 \
		if (!(Expression)) {                                                          \
			std::cout << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl; \
			std::cout << "CHECK FAILED: " << (TestName) << std::endl;                 \
			return false;                                                             \
		}                                                                             \
		std::cout << "CHECK PASSED: " << (TestName) << std::endl;                     \
	}

#define TEST_ENTRY(TestClass)                                                \
	int main(int Argc, char** Argv) {                                        \
		TestClass Test;                                                      \
		auto Args = std::span(Argv, size_t(Argc));                           \
		if (Args.size() > 1 && std::string_view(Args[1]) == "--benchmark") { \
			Test.Benchmark(Args);                                            \
			return 0;                                                        \
		} else {                                                             \
			return Test.Test(Args);                                          \
		}                                                                    \
	}
