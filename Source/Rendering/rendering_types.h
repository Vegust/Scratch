#pragma once

#include "basic.h"

enum class rendering_api : u8 {
	opengl
};

enum class view_mode : u8 { lit = 0, unlit = 1, depth = 2 };

struct render_state {
	rendering_api mApi{rendering_api::opengl};
	u32 mWidth{1000};
	u32 mHeight{800};
};