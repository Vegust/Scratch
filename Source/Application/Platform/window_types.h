#pragma once

#include "basic.h"
#include "Application/Input/input.h"
#include "Containers/dyn_array.h"

struct window_state {
	bool VSync{true};
	bool CursorEnabled{true};
	u32 Width{1000};
	u32 Height{800};
};