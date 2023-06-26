//
// Created by Vegust on 26.06.2023.
//

#include "light.h"

#include "core_types.h"

#include <array>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

static const char* TypeName(light_type Type)
{
	switch(Type)
	{
		case light_type::point:
			return "point";
		case light_type::directional:
			return "directional";
		case light_type::spot:
			return "spot";
	}
	std::unreachable();
}

void light::UIControlPanel()
{
	ImGui::Separator();

	constexpr std::array<light_type, 3> Types = { light_type::point, light_type::directional, light_type::spot};

	if (ImGui::BeginCombo("Light Type", TypeName(Type)))
	{
		for (uint64 i = 0; i < Types.size(); ++i)
		{
			bool bIsSelected = Type == Types[i];
			if (ImGui::Selectable(TypeName(Types[i]), bIsSelected))
			{
				Type = Types[i];
			}
		}
		ImGui::EndCombo();
	}

	ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(Diffuse));
	ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(Ambient));
	ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(Specular));
	
	if (Type == light_type::point)
	{
		ImGui::SliderFloat3("Light Position", glm::value_ptr(Position), -5.f, 5.f);
		ImGui::SliderFloat("Light Attenuation Radius", &AttenuationRadius, 0.f, 100.f);
	}
	else if (Type == light_type::directional)
	{
		ImGui::SliderFloat3("Light Direction", glm::value_ptr(Direction), -1.f, 1.f);
	}
	else if (Type == light_type::spot)
	{
		ImGui::SliderFloat3("Light Position", glm::value_ptr(Position), -5.f, 5.f);
		ImGui::SliderFloat3("Light Direction", glm::value_ptr(Direction), -1.f, 1.f);
		ImGui::SliderFloat("Light Attenuation Radius", &AttenuationRadius, 0.f, 100.f);
		ImGui::SliderFloat("Light Angular attenuation", &AngularAttenuation, 0.f, 180.f);
		ImGui::SliderFloat("Light Angular attenuation falloff start", &AngularAttenuationFalloffStart, 0.f, AngularAttenuation);
	}
}
