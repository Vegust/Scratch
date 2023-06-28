//
// Created by Vegust on 28.06.2023.
//

#pragma once

#include "core_types.h"
#include "vertex_buffer_layout.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/ext/vector_float3.hpp"
#include "glm/vec2.hpp"
SCRATCH_DISABLE_WARNINGS_END()

struct vertex
{
	glm::vec3 Position{0.f,0.f,0.f};
	glm::vec3 Normal{0.f,0.f,0.f};
	glm::vec2 UV{0.f,0.f};
	
	static vertex_buffer_layout GetLayout()
	{
		vertex_buffer_layout Layout;
		Layout.Push<float>(3); // position
		Layout.Push<float>(3); // normal
		Layout.Push<float>(2); // UV
		return Layout;
	}
};
