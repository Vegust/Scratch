#pragma once

#include "core_types.h"

struct world;
struct camera;

struct view {
	const camera& mCamera;
	const world& mWorld;
	vec4 mViewportRect{0,0,1000,800};
	view_mode mViewMode{view_mode::lit};
	float mZOrder = 1.f; // bigger rendered over lower
	// stuff like depth or post process also here
};