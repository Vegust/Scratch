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

	struct year_month_day {
		s64 Year;
		s64 Month;
		s64 Day;
	};

	constexpr static s64 NanosecondsPerTick = 100;
	constexpr static s64 TicksPerMicrosecond = 10;
	constexpr static s64 TicksPerMillisecond = 10000;
	constexpr static s64 TicksPerSecond = 10000000;
	constexpr static s64 TicksPerMinute = 600000000;
	constexpr static s64 TicksPerHour = 36000000000;
	constexpr static s64 TicksPerDay = 864000000000;
	constexpr static s64 TicksPerWeek = 6048000000000;
	constexpr static s64 TicksPerYear = 365 * TicksPerDay;

	// UTC
	static timestamp GetCurrentUTC();

	constexpr s64 GetYear() const {
		return GetYearMonthDay().Year;
	}

	constexpr s64 GetMonth() const {
		return GetYearMonthDay().Month;
	}

	constexpr s64 GetDay() const {
		return GetYearMonthDay().Day;
	}

	constexpr s64 GetHour() const {
		return GetTime().Ticks / TicksPerHour;
	}

	constexpr s64 GetMinute() const {
		return (Ticks % TicksPerHour) / TicksPerMinute;
	}

	constexpr s64 GetSecond() const {
		return (Ticks % TicksPerMinute) / TicksPerSecond;
	}

	constexpr s64 GetMillisecond() const {
		return (Ticks % TicksPerSecond) / TicksPerMillisecond;
	}

	constexpr s64 GetMicrosecond() const {
		return (Ticks % TicksPerMillisecond) / TicksPerMicrosecond;
	}

	constexpr s64 GetNanosecond() const {
		return (Ticks % TicksPerMicrosecond) * NanosecondsPerTick;
	}

	// truncated to latest whole day
	constexpr timestamp GetDate() const {
		return timestamp{Ticks - (Ticks % TicksPerDay)};
	}

	// from 0 to TicksPerDay - 1
	constexpr timestamp GetTime() const {
		return timestamp(Ticks % TicksPerDay);
	}

	constexpr timestamp::year_month_day GetYearMonthDay() const {
		// Based on FORTRAN code in:
		// Fliegel, H. F. and van Flandern, T. C.,
		// Communications of the ACM, Vol. 11, No. 10 (October 1968).

		s32 i, j, k, l, n;

		l = (s32) (GetDate().GetJulianDay() + 0.5) + 68569;
		n = 4 * l / 146097;
		l = l - (146097 * n + 3) / 4;
		i = 4000 * (l + 1) / 1461001;
		l = l - 1461 * i / 4 + 31;
		j = 80 * l / 2447;
		k = l - 2447 * j / 80;
		l = j / 11;
		j = j + 2 - 12 * l;
		i = 100 * (n - 49) + i + l;

		year_month_day Result;
		Result.Year = i;
		Result.Month = j;
		Result.Day = k;
		return Result;
	}

	constexpr double GetJulianDay() const {
		return 1721425.5 + (double) (Ticks / TicksPerDay) + ((double) (Ticks % TicksPerDay) / TicksPerDay);
	}

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
