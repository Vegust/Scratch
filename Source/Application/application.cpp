#pragma once

#include "application.h"
#include "Application/Platform/platform.h"

static float UpdateTime(float& LastFrameTime) {
	const float NewTime = platform::GetTime();
	const float DeltaTime = NewTime - LastFrameTime;
	LastFrameTime = NewTime;
	return DeltaTime;
}

application::application() {
	mWindow.Init(this, 1000, 800);
	mRenderer.Init(1000, 800);
	mUI.Init(this, mRenderer.mRenderingApi);
	mGame.Init(mRenderer);
	mCurrentTime = platform::GetTime();
}

void application::Run() {
	while (!mWindow.ShouldClose()) {
		float DeltaTime = UpdateTime(mCurrentTime);
		mInputState.SavePreviousFrameInput();
		mWindow.ProcessEvents();
		mUI.StartNewFrame();
		mGame.Update(DeltaTime, mRenderer, this);
		mRenderer.RenderView();
		mUI.Render();
		mWindow.SwapBuffers();
	}
}
