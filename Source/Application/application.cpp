#pragma once

#include "application.h"
#include "Application/Platform/platform.h"

struct time_update_result {
	float DeltaTime{0.f};
	float NewTime{0.f};
};

static time_update_result UpdateTime(float LastFrameTime) {
	const float NewTime = platform::GetTime();
	const float DeltaTime = NewTime - LastFrameTime;
	return {DeltaTime, NewTime};
}

application::application(u32 WindowWidth, u32 WindowHeight)
	: mWindow{WindowWidth, WindowHeight}, mRenderer{WindowWidth, WindowHeight}, mGame{}, mTime{0.f} {
}

bool application::RunOneFrame() {
	const auto [DeltaTime, Time] = UpdateTime(mTime);
	const auto [Input, WindowMessages, WindowState] = mWindow.ProcessExternalEvents();
	const auto RenderState = mRenderer.HandleMessages(WindowMessages);
	const auto [Views, UIData, GameMessages] = mGame.Step(Time, DeltaTime, Input, WindowState, RenderState);
	const auto MessagesLeft = mWindow.HandleMessages(GameMessages);
	mRenderer.HandleMessages(MessagesLeft);
	mRenderer.RenderViews(Views);
	mRenderer.RenderUI(UIData);
	mWindow.SwapBuffers();
	mTime = Time;
	return !mWindow.ShouldClose();
}
