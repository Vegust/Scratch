#pragma once

#include "Application/Platform/window_types.h"
#include "Rendering/rendering_types.h"

enum class app_message_type : u8 { window_close, window_vsync, window_cursor, render_resize };

struct app_message_window_close {};

struct app_message_window_vsync {
	bool mVSync{true};
};

struct app_message_window_cursor {
	bool mCursor{true};
};

struct app_message_render_resize {
	u32 NewWidth{1000};
	u32 NewHeight{800};
};

struct app_message {
	union {
		app_message_window_close mWindowClose{};
		app_message_window_vsync mWindowVsync;
		app_message_window_cursor mWindowCursor;
		app_message_render_resize mRenderResize;
	};

	app_message_type mType{};
};