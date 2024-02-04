#pragma once

#include "basic.h"

struct timestamp {
	// Since 01-01-0001, 1 Tick = 100 nanoseconds
	s64 Ticks{0};

	constexpr timestamp() = default;
	constexpr explicit timestamp(s64 InTicks) : Ticks{InTicks} {
	}

	// UTC
	static timestamp GetCurrent();

	// truncated to latest whole day
	timestamp GetDate() const;
	// from 0 to TicksPerDay - 1
	timestamp GetTime() const;
	
	struct year_month_day {
		s64 Year;
		s64 Month;
		s64 Day;
	};
	year_month_day GetYearMonthDay() const;
	
	double GetJulianDay() const;

	s64 GetYear() const;
	s64 GetMonth() const;
	s64 GetDay() const;
	s64 GetHour() const;
	s64 GetMinute() const;
	s64 GetSecond() const;
	s64 GetMillisecond() const;
	s64 GetMicrosecond() const;
	s64 GetNanosecond() const;

	constexpr static s64 NanosecondsPerTick = 100;
	constexpr static s64 TicksPerMicrosecond = 10;
	constexpr static s64 TicksPerMillisecond = 10000;
	constexpr static s64 TicksPerSecond = 10000000;
	constexpr static s64 TicksPerMinute = 600000000;
	constexpr static s64 TicksPerHour = 36000000000;
	constexpr static s64 TicksPerDay = 864000000000;
	constexpr static s64 TicksPerWeek = 6048000000000;
	constexpr static s64 TicksPerYear = 365 * TicksPerDay;
};
