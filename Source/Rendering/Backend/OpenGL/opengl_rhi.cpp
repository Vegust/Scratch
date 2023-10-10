#include "opengl_rhi.h"

#include "Application/Platform/platform.h"
#include "Rendering/rendering_types.h"
#include "glad/glad.h"
#include "backends/imgui_impl_opengl3.h"

void opengl_rhi::Init() {
	auto GLLoadingResult = gladLoadGLLoader(
		reinterpret_cast<GLADloadproc>(platform::GetApiLoadingFunction(rendering_api::opengl)));
	CHECK(GLLoadingResult);
}

void opengl_rhi::InitUIData(const void* Data) {
	const char* OpenGLVersion = reinterpret_cast<const char*>(Data);
	ImGui_ImplOpenGL3_Init(OpenGLVersion);
	ImGui_ImplOpenGL3_NewFrame();
}

void opengl_rhi::RenderUI() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}