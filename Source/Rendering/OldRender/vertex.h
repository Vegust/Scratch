#pragma once

#include "core_types.h"
#include "vertex_buffer_layout.h"

struct vertex {
	vec3 Position{0.f, 0.f, 0.f};
	vec3 Normal{0.f, 0.f, 0.f};
	vec2 UV{0.f, 0.f};
	vec3 Tangent{0.f,0.f,0.f};

	constexpr vertex() = default;
	constexpr vertex(
		float V1,
		float V2,
		float V3,
		float V4,
		float V5,
		float V6,
		float V7,
		float V8,
		float V9 = 0.f,
		float V10 = 0.f,
		float V11 = 0.f)
		: Position(V1, V2, V3), Normal(V4, V5, V6), UV(V7, V8), Tangent(V9, V10, V11)
	{
	}

	static vertex_buffer_layout GetLayout() {
		vertex_buffer_layout Layout;
		Layout.Push<float>(3);	  // position
		Layout.Push<float>(3);	  // normal
		Layout.Push<float>(2);	  // UV
		Layout.Push<float>(3);	  // tangent
		return Layout;
	}
};
