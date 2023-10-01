#include "light.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

static const char* TypeName(light_type Type) {
	switch (Type) {
		case light_type::point:
			return "point";
		case light_type::directional:
			return "directional";
		case light_type::spot:
			return "spot";
	}
	std::unreachable();
}

void light::UIControlPanel(const str& Prefix) {
	ImGui::Separator();

	constexpr array<light_type, 3> Types = {
		light_type::point, light_type::directional, light_type::spot};

	if (ImGui::BeginCombo((Prefix + "Light Type").Raw(), TypeName(mType))) {
		for (u32 i = 0; i < Types.Size(); ++i) {
			bool bIsSelected = mType == Types[i];
			if (ImGui::Selectable(TypeName(Types[i]), bIsSelected)) {
				mType = Types[i];
			}
		}
		ImGui::EndCombo();
	}

	ImGui::ColorEdit3((Prefix + "Light Diffuse Color").Raw(), glm::value_ptr(mDiffuse));
	ImGui::ColorEdit3((Prefix + "Light Ambient Color").Raw(), glm::value_ptr(mAmbient));
	ImGui::ColorEdit3((Prefix + "Light Specular Color").Raw(), glm::value_ptr(mSpecular));

	if (mType == light_type::point) {
		ImGui::InputFloat3((Prefix + "Light Position").Raw(), glm::value_ptr(mPosition));
		ImGui::SliderFloat3(
			(Prefix + "Position Slider").Raw(), glm::value_ptr(mPosition), -20.f, 20.f);
		ImGui::InputFloat((Prefix + "Light Attenuation Radius").Raw(), &mAttenuationRadius);
	} else if (mType == light_type::directional) {
		ImGui::SliderFloat3(
			(Prefix + "Light Direction").Raw(), glm::value_ptr(mDirection), -1.f, 1.f);
	} else if (mType == light_type::spot) {
		ImGui::InputFloat3((Prefix + "Light Position").Raw(), glm::value_ptr(mPosition));
		ImGui::SliderFloat3(
			(Prefix + "Position Slider").Raw(), glm::value_ptr(mPosition), -20.f, 20.f);
		ImGui::SliderFloat3(
			(Prefix + "Light Direction").Raw(), glm::value_ptr(mDirection), -1.f, 1.f);
		ImGui::InputFloat((Prefix + "Light Attenuation Radius").Raw(), &mAttenuationRadius);
		ImGui::SliderFloat(
			(Prefix + "Light Angular attenuation").Raw(), &mAngularAttenuation, 0.f, 180.f);
		ImGui::SliderFloat(
			(Prefix + "Light Angular attenuation falloff start").Raw(),
			&mAngularAttenuationFalloffStart,
			0.f,
			mAngularAttenuation);
	}
}
