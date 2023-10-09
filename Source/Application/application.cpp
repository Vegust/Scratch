#pragma once

#include "application.h"
#include "Application/Platform/platform.h"

#include "imgui.h"

void application::Run() {
	while (!mWindow.ShouldClose()) {
		const float NewTime = platform::GetTime();
		mDeltaTime = NewTime - mLastFrameTime;
		mLastFrameTime = NewTime;

		mInputState.OnNewFrame();
		mWindow.ProcessEvents(mOldRenderer, mDeltaTime);

		mUI.OnNewFrame();
		mUI.StartDebugWindow(mDeltaTime);
		if (ImGui::Checkbox("VSync", &mWindow.mVSync)) {
			mWindow.SetVSync(mWindow.mVSync);
		}
		mOldRenderer.UIRendererControls();
		TestMap.OnIMGuiRender(mOldRenderer);
		mUI.EndDebugWindow();

		// ---- temporary place to handle game input ----
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
		// -----------------------------------------------

		TestMap.OnUpdate(mDeltaTime);
		TestMap.OnRender(mOldRenderer);
		mUI.Render();
		mWindow.SwapBuffers();
	}
}

application::application() {
	mWindow.Init(this, 1000, 800);
	mRenderingContext.Init(mRenderingApi, mWindow);
	mOldRenderer.Init(800, 1000);
	mUI.Init(this);
	mLastFrameTime = platform::GetTime();
	TestMap.Init(mOldRenderer);
}
