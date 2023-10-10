#include "ui.h"
#include "imgui.h"
#include "Rendering/rendering_types.h"
#include "Application/Platform/platform.h"
#include "Application/application.h"
#include "Rendering/Backend/dynamic_rhi.h"

void ui::Init(application* App, rendering_api Api) {
	mParentApp = App;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
	platform::InitUi(*this, mParentApp->mWindow, Api);
	const char* OpenGLVersion = "#version 460";
	mParentApp->mRenderer.mRHI->InitUIData(OpenGLVersion);
}

void ui::StartNewFrame() {
	platform::UpdateUi(*this, mParentApp->mWindow);
	ImGui::NewFrame();
}

void ui::StartDebugWindow(float DeltaTime) {
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowFontScale(1.5f);
	ImGui::Text("Frame time %.4f ms", 1000.0 * DeltaTime);
	ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
}

void ui::EndDebugWindow() {
	ImGui::End();
}

void ui::Render() {
	ImGui::Render();
	mParentApp->mRenderer.mRHI->RenderUI();
}

bool ui::ConsumesKeyboardInput() const {
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureKeyboard;
}

bool ui::ConsumesMouseInput() const {
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse;
}