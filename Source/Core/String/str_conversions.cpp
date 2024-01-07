#include "str_conversions.h"
#include "fast_float.h"

namespace strings {
result<float> GetFloat(str_view String) {
	float Result;
	auto Answer = fast_float::from_chars(String.GetData(),String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return common_errors::who_knows; // TODO: actually look at error
	}
	return result<float>(Result);
}

result<double> GetDouble(str_view String) {
	double Result;
	auto Answer = fast_float::from_chars(String.GetData(),String.GetData() + String.GetSize(), Result);
	if (static_cast<s32>(Answer.ec)) {
		return common_errors::who_knows; // TODO: actually look at error
	}
	return result<double>(Result);
}
}