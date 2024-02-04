#include "logs.h"
#include "Time/timestamp.h"

void logs::Log(
	logs::color Color,
	str_view VerbosityString,
	atom Category,
	str_view FormatString,
	span<strings::format_argument> ArgumentArray) {
	const index CategoryLength = Category.ToStr().GetByteLength();
	const index InputLength = strings::GetFormatLength(FormatString, ArgumentArray);
	const timestamp CurrentTime = timestamp::GetCurrent();
	const index TimeLength = strings::default_timestamp_format::GetCharSize(CurrentTime);
	const index TotalLength = TimeLength + 3 + VerbosityString.GetSize() + 3 + CategoryLength + 3 + InputLength;
	str LogString;
	LogString.Reserve(TotalLength + 1);
	strings::default_timestamp_format::Write(LogString.GetData(), TimeLength, CurrentTime);
	LogString.OverwriteSize(TimeLength + 1);
	LogString += " | ";
	LogString += VerbosityString;
	LogString += " | ";
	LogString += Category.ToStr();
	LogString += " | ";
	strings::WriteFormat(LogString.GetData() + LogString.GetByteLength(), FormatString, ArgumentArray);
	LogString.OverwriteSize(TotalLength + 1);
	LogString[TotalLength] = 0;
	switch (Color) {
		case color::white:
			break;
		case color::yellow:
			std::cout << "\033[1;33m";
			break;
		case color::red:
			std::cout << "\033[1;31m";
			break;
	}
	std::cout << LogString << "\n";	   // TODO real output
	if (Color != color::white) {
		std::cout << "\033[1;0m";
	}
}
