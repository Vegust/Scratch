#include "ui.h"
#include "Application/application.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

void ui::Init(application* App) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
	// OpenGL hardcode
	ImGui_ImplGlfw_InitForOpenGL(App->GetWindow().mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void ui::OnNewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
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
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
