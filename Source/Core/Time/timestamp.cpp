#include "timestamp.h"
#include "Application/Platform/platform.h"
#include "Containers/array.h"

constexpr array<s64, 13> DaysPerMonth = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr array<s64, 13> DaysToMonth = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

constexpr bool IsLeapYear(s64 Year) {
	if ((Year % 4) == 0) {
		return (((Year % 100) != 0) || ((Year % 400) == 0));
	}
	return false;
}

constexpr s64 DateTimeToTicks(
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
	return TotalDays * timestamp::TicksPerDay + Hour * timestamp::TicksPerHour + Minute * timestamp::TicksPerMinute +
		   Second * timestamp::TicksPerSecond + Millisecond * timestamp::TicksPerMillisecond;
}

timestamp timestamp::GetCurrent() {
	timestamp Current;
	s64 UTCTicks = platform::GetUTC();
	// January 1, 1601 (UTC).
	constexpr s64 InitialTicks = DateTimeToTicks(1601, 1, 1);
	Current.Ticks = UTCTicks + InitialTicks;
	return Current;
}

s64 timestamp::GetYear() const {
	return GetYearMonthDay().Year;
}

s64 timestamp::GetMonth() const {
	return GetYearMonthDay().Month;
}

s64 timestamp::GetDay() const {
	return GetYearMonthDay().Day;
}

s64 timestamp::GetHour() const {
	return GetTime().Ticks / TicksPerHour;
}

s64 timestamp::GetMinute() const {
	return (Ticks % TicksPerHour) / TicksPerMinute;
}

s64 timestamp::GetSecond() const {
	return (Ticks % TicksPerMinute) / TicksPerSecond;
}

s64 timestamp::GetMillisecond() const {
	return (Ticks % TicksPerSecond) / TicksPerMillisecond;
}

s64 timestamp::GetMicrosecond() const {
	return (Ticks % TicksPerMillisecond) / TicksPerMicrosecond;
}

s64 timestamp::GetNanosecond() const {
	return (Ticks % TicksPerMicrosecond) * NanosecondsPerTick;
}

timestamp timestamp::GetDate() const {
	return timestamp{Ticks - (Ticks % TicksPerDay)};
}

timestamp timestamp::GetTime() const {
	return timestamp(Ticks % TicksPerDay);
}

timestamp::year_month_day timestamp::GetYearMonthDay() const {
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

double timestamp::GetJulianDay() const {
	return 1721425.5 + (double) (Ticks / TicksPerDay) + ((double) (Ticks % TicksPerDay) / TicksPerDay);
}
