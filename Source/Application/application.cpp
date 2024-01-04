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

application::application(const application_settings& InSettings)
	: Settings{InSettings}
	, Window{InSettings.WindowWidth, InSettings.WindowHeight}
	, Renderer{InSettings.WindowWidth, InSettings.WindowHeight}
	, Game{}
	, Time{0.f} {
}

bool application::RunOneFrame() {
	const auto [DeltaTime, NewTime] = UpdateTime(Time);
	const auto [Input, WindowMessages, WindowState] = Window.ProcessExternalEvents();
	const auto RenderState = Renderer.HandleMessages(WindowMessages);
	const auto [Views, UIData, GameMessages] = Game.Step(NewTime, DeltaTime, Input, WindowState, RenderState);
	const auto MessagesLeft = Window.HandleMessages(GameMessages);
	Renderer.HandleMessages(MessagesLeft);
	Renderer.RenderViews(Views);
	Renderer.RenderUI(UIData);
	Window.SwapBuffers();
	Time = NewTime;
	return !Window.ShouldClose();
}
