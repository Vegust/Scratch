#pragma once

#include "core_types.h"

#include "glm/glm.hpp"
#include "Containers/str.h"

enum class light_type { point = 0, directional = 1, spot = 2 };

struct light {
	light_type mType{light_type::point};
	glm::vec3 mColor{1.f, 1.f, 1.f};
	float mAmbientStrength = 0.1f;
	glm::vec3 mPosition{0.f, 0.f, 0.f};
	glm::vec3 mDirection{-0.2f, -1.0f, -0.3f};
	float mAttenuationRadius{20.f};
	float mAngularAttenuation{12.5f};
	float mAngularAttenuationFalloffStart{10.0f};

	void UIControlPanel(const str& Prefix);
};
