#include "game.h"
#include "Rendering/renderer.h"
#include "Application/application.h"
#include "imgui.h"

void game::Init(renderer& Renderer) {
	mTestMap.Init(Renderer.mOldRenderer);
}

void game::Update(float DeltaTime, renderer& Renderer, application* App) {
	App->mUI.StartDebugWindow(DeltaTime);
	bool VSync = App->mWindow.GetVSync();
	if (ImGui::Checkbox("VSync", &VSync)) {
		App->mWindow.SetVSync(VSync);
	}
	App->mRenderer.mOldRenderer.UIRendererControls();
	mTestMap.OnIMGuiRender(App->mRenderer.mOldRenderer);
	App->mUI.EndDebugWindow();
	HandleInput(DeltaTime, App);
	mTestMap.OnUpdate(DeltaTime);
	mTestMap.OnRender(Renderer.mOldRenderer);
}

void game::HandleInput(float DeltaTime, application* App) {
	if (!App->mUI.ConsumesKeyboardInput() && !App->mUI.ConsumesMouseInput()) {
		if (App->mInputState.PressedThisFrame(input_key::keyboard_escape)) {
			App->mWindow.CloseWindow();
		}
		// for now camera = controller
		const bool CursorHeld = App->mInputState.Pressed(input_key::mouse_left);
		App->mWindow.SetCursorEnabled(!CursorHeld);
		if (CursorHeld) {
			App->mRenderer.mOldRenderer.mCamera.ProcessInput(App->mInputState, DeltaTime);
		}
	}
}
