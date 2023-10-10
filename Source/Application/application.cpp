#pragma once

#include "application.h"
#include "Application/Platform/platform.h"

#include "imgui.h"
#include "Rendering/Backend/OpenGL/opengl_rhi.h"

application::application() {
	mWindow.Init(this, 1000, 800);
	mRHI = std::make_unique<opengl_rhi>();
	mRHI->Init();
	mOldRenderer.Init(800, 1000);
	mUI.Init(this, mRenderingApi);
	mTestMap.Init(mOldRenderer);
	mLastFrameTime = platform::GetTime();
}

void application::Run() {
	while (!mWindow.ShouldClose()) {
		const float NewTime = platform::GetTime();
		mDeltaTime = NewTime - mLastFrameTime;
		mLastFrameTime = NewTime;

		mInputState.OnNewFrame();
		mWindow.ProcessEvents();

		mUI.OnNewFrame();
		mUI.StartDebugWindow(mDeltaTime);
		bool VSync = mWindow.GetVSync();
		if (ImGui::Checkbox("VSync", &VSync)) {
			mWindow.SetVSync(VSync);
		}
		mOldRenderer.UIRendererControls();
		mTestMap.OnIMGuiRender(mOldRenderer);
		mUI.EndDebugWindow();

		// ---- temporary place to handle game input ----
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
			if (mInputState.PressedThisFrame(input_key::keyboard_escape)) {
				mWindow.CloseWindow();
			}
			// for now camera = controller
			if (!mOldRenderer.mCustomCamera.expired()) {
				const bool CursorHeld = mInputState.Pressed(input_key::mouse_left);
				mWindow.SetCursorEnabled(!CursorHeld);
				if (CursorHeld) {
					auto CameraHandle = mOldRenderer.mCustomCamera.lock();
					CameraHandle->ProcessInput(mInputState, mDeltaTime);
				}
			}
		}
		// -----------------------------------------------

		mTestMap.OnUpdate(mDeltaTime);
		mTestMap.OnRender(mOldRenderer);
		mUI.Render();
		mWindow.SwapBuffers();
	}
}
