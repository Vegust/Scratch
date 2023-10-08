#pragma once

#include "imgui.h"
#include "application.h"
#include "Application/Platform/utils.h"

// TODO remove -------------------------
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// -------------------------------------

void application::Run() {
	while (!mWindow.ShouldClose()) {
		const float NewTime = platform_utils::GetTime();
		const double DeltaTime = NewTime - LastFrameTime;
		LastFrameTime = NewTime;
		
		mWindow.ProcessEvents();

		TestMap.OnUpdate(DeltaTime);

		renderer::Clear();
		TestMap.OnRender(mOldRenderer);

		// TODO remove -------
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(
			"Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("Frame time %.4f ms", 1000.0 * DeltaTime);
		ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
		{
			bool VSync = false;
			if (ImGui::Checkbox("VSync", &VSync)) {
				mWindow.SetVSync(VSync);
			}
		}
		mOldRenderer.UIRendererControls();
		TestMap.OnIMGuiRender(mOldRenderer);

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// -------------------

		mWindow.SwapBuffers();
	}
}

application::application() {
	mWindow.Init(this, 1000, 800);
	mRenderingContext.Init(mRenderingApi, mWindow);
	mOldRenderer.Init(1000, 800);

	// ----- Raw ImGUI for now, TODO remove -------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
	// OpenGL hardcode
	ImGui_ImplGlfw_InitForOpenGL(mWindow.mWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	// --------------------------------------------------

	LastFrameTime = platform_utils::GetTime();
	TestMap.Init(mOldRenderer);
}
