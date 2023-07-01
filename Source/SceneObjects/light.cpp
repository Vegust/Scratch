//
// Created by Vegust on 26.06.2023.
//

#include "light.h"

#include "core_types.h"

#include <array>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
SCRATCH_DISABLE_WARNINGS_END()

static const char* TypeName(light_type Type)
{
	switch (Type)
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

void light::UIControlPanel(std::string_view Prefix)
{
	ImGui::Separator();

	constexpr std::array<light_type, 3> Types = {
		light_type::point, light_type::directional, light_type::spot};

	if (ImGui::BeginCombo(std::string{Prefix}.append("Light Type").c_str(), TypeName(Type)))
	{
		for (uint32 i = 0; i < Types.size(); ++i)
		{
			bool bIsSelected = Type == Types[i];
			if (ImGui::Selectable(TypeName(Types[i]), bIsSelected))
			{
				Type = Types[i];
			}
		}
		ImGui::EndCombo();
	}

	ImGui::ColorEdit3(
		std::string{Prefix}.append("Light Diffuse Color").c_str(), glm::value_ptr(Diffuse));
	ImGui::ColorEdit3(
		std::string{Prefix}.append("Light Ambient Color").c_str(), glm::value_ptr(Ambient));
	ImGui::ColorEdit3(
		std::string{Prefix}.append("Light Specular Color").c_str(), glm::value_ptr(Specular));

	if (Type == light_type::point)
	{
		ImGui::InputFloat3(
			std::string{Prefix}.append("Light Position").c_str(),
			glm::value_ptr(Position));
		ImGui::InputFloat(
			std::string{Prefix}.append("Light Attenuation Radius").c_str(),
			&AttenuationRadius);
	}
	else if (Type == light_type::directional)
	{
		ImGui::SliderFloat3(
			std::string{Prefix}.append("Light Direction").c_str(),
			glm::value_ptr(Direction),
			-1.f,
			1.f);
	}
	else if (Type == light_type::spot)
	{
		ImGui::InputFloat3(
			std::string{Prefix}.append("Light Position").c_str(),
			glm::value_ptr(Position));
		ImGui::SliderFloat3(
			std::string{Prefix}.append("Light Direction").c_str(),
			glm::value_ptr(Direction),
			-1.f,
			1.f);
		ImGui::InputFloat(
			std::string{Prefix}.append("Light Attenuation Radius").c_str(),
			&AttenuationRadius);
		ImGui::SliderFloat(
			std::string{Prefix}.append("Light Angular attenuation").c_str(),
			&AngularAttenuation,
			0.f,
			180.f);
		ImGui::SliderFloat(
			std::string{Prefix}.append("Light Angular attenuation falloff start").c_str(),
			&AngularAttenuationFalloffStart,
			0.f,
			AngularAttenuation);
	}
}
