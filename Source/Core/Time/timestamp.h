#pragma once

#include "basic.h"
#include "Containers/array.h"

struct timestamp {
	// Since 01-01-0001, 1 Tick = 100 nanoseconds
	s64 Ticks{0};

	static constexpr array<s64, 13> DaysPerMonth = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static constexpr array<s64, 13> DaysToMonth = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

	constexpr timestamp() = default;

	constexpr explicit timestamp(s64 InTicks) : Ticks{InTicks} {
	}

	// UTC
	static timestamp GetCurrentUTC();
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

	static constexpr s64 DateTimeToTicks(
		s64 Year,
		s64 Month,
		s64 Day,
		s64 Hour = 0,
		s64 Minute = 0,
		s64 Second = 0,
		s64 Millisecond = 0) {
		s64 TotalDays = 0;
		if ((Month > 2) && IsLeapYear(Year)) {
			++TotalDays;
		}
		// Year, Month and Day counted before they actually finish
		--Year;
		--Month;
		--Day;
		TotalDays += Year * 365;
		TotalDays += Year / 4;
		TotalDays -= Year / 100;
		TotalDays += Year / 400;
		TotalDays += DaysToMonth[Month];
		TotalDays += Day;
		return TotalDays * timestamp::TicksPerDay + Hour * timestamp::TicksPerHour +
			   Minute * timestamp::TicksPerMinute + Second * timestamp::TicksPerSecond +
			   Millisecond * timestamp::TicksPerMillisecond;
	}

	static constexpr bool IsLeapYear(s64 Year) {
		if ((Year % 4) == 0) {
			return (((Year % 100) != 0) || ((Year % 400) == 0));
		}
		return false;
	}
};

struct timezone {
	// Difference with UTC time, 1 Tick = 100 nanoseconds
	s64 TickDiff{0};
	constexpr timezone() = default;

	constexpr explicit timezone(s64 InTickDiff) : TickDiff{InTickDiff} {
	}

	timestamp Apply(timestamp Timestamp) {
		return timestamp{Timestamp.Ticks - TickDiff};
	}
};
