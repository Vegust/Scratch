//
// Created by Vegust on 26.06.2023.
//

#pragma once

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

class phong_material
{
public:
	glm::vec3 Ambient{1.f,0.5f,0.31f};
	glm::vec3 Diffuse{1.f,0.5f,0.31f};
	glm::vec3 Specular{0.5f,0.5f,0.5f};
	float Shininess{32.f};
};
