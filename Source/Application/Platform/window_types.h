#pragma once

#include "core_types.h"
#include "Application/Input/input.h"
#include "Containers/dyn_array.h"

struct window_state {
	bool mVSync{true};
	bool mCursorEnabled{true};
	u32 mWidth{1000};
	u32 mHeight{800};
};