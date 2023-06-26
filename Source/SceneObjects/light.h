//
// Created by Vegust on 26.06.2023.
//

#pragma once

#include "core_types.h"

#include <string>
#include <utility>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
SCRATCH_DISABLE_WARNINGS_END()

enum class light_type
{
	point = 0,
	directional = 1,
	spot = 2
};

class light
{
public:
	light_type Type{light_type::point};
	glm::vec3 Position{0.f,0.f,0.f};
	glm::vec3 Ambient{0.1f,0.1f,0.1f};
	glm::vec3 Diffuse{1.f,1.f,1.f};
	glm::vec3 Specular{1.f,1.f,1.f};
	
	glm::vec3 Direction{0.f,0.f,-1.f};
	
	float AttenuationRadius{20.f};
	
	float AngularAttenuation{12.5f};
	float AngularAttenuationFalloffStart{10.0f};
	
	void UIControlPanel();
};
