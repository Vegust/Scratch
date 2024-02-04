#pragma once

#include "basic.h"
#include <chrono>

struct timer {
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
	time_point StartTime{};
	float Accumulator{0};
	
	void Start() {
		StartTime = std::chrono::high_resolution_clock::now();
	}

	void Stop() {
		time_point End = std::chrono::high_resolution_clock::now();
		s64 Time = std::chrono::duration_cast<std::chrono::microseconds>(End - StartTime).count();
		Accumulator += Time / 1000.f;
	}

	[[nodiscard]] float Result() const {
		return Accumulator;
	}

	void Offset(const timer& OffsetTimer) {
		Accumulator -= OffsetTimer.Accumulator;
	}

	void Clear() {
		StartTime = {};
		Accumulator = 0;
	}
};