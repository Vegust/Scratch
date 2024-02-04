#include "logs.h"

void logs::Log(
	logs::color Color,
	str_view VerbosityString,
	atom Category,
	str_view FormatString,
	span<strings::format_argument> ArgumentArray) {
	const index CategoryLength = Category.ToStr().GetByteLength();
	const index InputLength = strings::GetFormatLength(FormatString, ArgumentArray);
	const str_view TimeString{"03.02.2024 16:30:59.1005"};	  // TODO real time
	const index TotalLength =
		TimeString.GetSize() + 3 + VerbosityString.GetSize() + 3 + CategoryLength + 3 + InputLength;
	str LogString;
	LogString.Reserve(TotalLength + 1);
	LogString += TimeString;
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
