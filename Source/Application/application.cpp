#pragma once

#include "imgui.h"
#include "application.h"
#include "Application/Platform/platform.h"

// TODO remove -------------------------
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// -------------------------------------

void application::Run() {
	while (!mWindow.ShouldClose()) {
		const float NewTime = platform::GetTime();
		mDeltaTime = NewTime - mLastFrameTime;
		mLastFrameTime = NewTime;

		mInputState.OnNewFrame();
		mWindow.ProcessEvents(mOldRenderer, mDeltaTime);

		// ---- temporary place to handle input ----
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
			if (GetInputState().PressedThisFrame(input_key::keyboard_escape)) {
				mWindow.CloseWindow();
			}
			// for now camera = controller
			if (!mOldRenderer.mCustomCamera.expired()) {
				const bool CursorHeld = GetInputState().Pressed(input_key::mouse_left);
				mWindow.SetCursorEnabled(!CursorHeld);
				if (CursorHeld) {
					auto CameraHandle = mOldRenderer.mCustomCamera.lock();
					CameraHandle->ProcessInput(GetInputState(), mDeltaTime);
				}
			}
		}
		// -----------------------------------------

		TestMap.OnUpdate(mDeltaTime);
		renderer::Clear();
		TestMap.OnRender(mOldRenderer);

		// TODO remove -------
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin(
			"Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("Frame time %.4f ms", 1000.0 * mDeltaTime);
		ImGui::Text("%.1f FPS", static_cast<double>(ImGui::GetIO().Framerate));
		{
			if (ImGui::Checkbox("VSync", &mWindow.mVSync)) {
				mWindow.SetVSync(mWindow.mVSync);
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
	mOldRenderer.Init(800, 1000);

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

	mLastFrameTime = platform::GetTime();
	TestMap.Init(mOldRenderer);
}
