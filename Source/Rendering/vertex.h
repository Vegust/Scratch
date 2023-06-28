//
// Created by Vegust on 28.06.2023.
//

#pragma once

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/ext/vector_float3.hpp"
#include "glm/vec2.hpp"
SCRATCH_DISABLE_WARNINGS_END()

struct vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;
};
