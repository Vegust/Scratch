#include "ui_data.h"

//void ui::Init(rendering_api Api) {
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO();
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
//	io.IniFilename = nullptr;
//	ImGui::StyleColorsDark();
//	platform::InitUi(*this, mParentApp->mWindow, Api);
//	const char* OpenGLVersion = "#version 460";
//	mParentApp->mRenderer.mRHI->InitUIData(OpenGLVersion);
//}

//void ui::StartNewFrame() {
//	platform::UpdateUi(*this, mParentApp->mWindow);
//	ImGui::NewFrame();
//}

void ui_data::StartDebugWindow(float DeltaTime) {
//	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
//	ImGui::SetWindowFontScale(1.5f);
//	ImGui::Text("Frame time %.4f ms", 1000.0 * DeltaTime);
//	ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
}

void ui_data::EndDebugWindow() {
	//ImGui::End();
}

bool ui_data::ConsumesKeyboardInput() const {
	return false;
//	ImGuiIO& io = ImGui::GetIO();
//	return io.WantCaptureKeyboard;
}

bool ui_data::ConsumesMouseInput() const {
	return false;
//	ImGuiIO& io = ImGui::GetIO();
//	return io.WantCaptureMouse;
}