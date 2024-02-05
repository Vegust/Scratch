#include "timestamp.h"
#include "Application/Platform/platform.h"
#include "Containers/array.h"

timestamp timestamp::GetCurrentUTC() {
	return platform::GetUTC();
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
