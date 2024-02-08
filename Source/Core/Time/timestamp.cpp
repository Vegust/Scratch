#include "timestamp.h"
#include "Application/Platform/platform.h"
#include "Containers/array.h"


timestamp timestamp::GetCurrentUTC() {
	return platform::GetUTC();
}