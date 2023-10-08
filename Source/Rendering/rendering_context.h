#pragma once

#include "rendering_types.h"
#include "Application/Platform/window.h"

class rendering_context {
public:
	void Init(rendering_api Api, window& Window);
};