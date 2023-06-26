//
// Created by Vegust on 26.06.2023.
//

#pragma once

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

class light
{
public:
	glm::vec3 Position{0.f,0.f,0.f};
	glm::vec3 Ambient{0.1f,0.1f,0.1f};
	glm::vec3 Diffuse{1.f,1.f,1.f};
	glm::vec3 Specular{1.f,1.f,1.f};
};
