#include "game.h"

static ui_data UpdateUi() {
	//	mUI.Init(this, Renderer.mRenderingApi);
	//	mUI.StartNewFrame();
	//	mUI.StartDebugWindow(DeltaTime);
	//	bool VSync = App->mWindow.GetVSync();
	//	if (ImGui::Checkbox("VSync", &VSync)) {
	//		App->mWindow.SetVSync(VSync);
	//	}
	//	App->mRenderer.mOldRenderer.UIRendererControls();
	//	mTestMap.OnIMGuiRender(App->mRenderer.mOldRenderer);
	//	mUI.EndDebugWindow();
	//	if (!mUI.ConsumesKeyboardInput() && !App->mUI.ConsumesMouseInput()) {
	//		if (App->mInputState.PressedThisFrame(input_key::keyboard_escape)) {
	//			App->mWindow.CloseWindow();
	//		}
	//		// for now camera = controller
	//		const bool CursorHeld = App->mInputState.Pressed(input_key::mouse_left);
	//		App->mWindow.SetCursorEnabled(!CursorHeld);
	//		if (CursorHeld) {
	//			App->mRenderer.mOldRenderer.mCamera.ProcessInput(App->mInputState, DeltaTime);
	//		}
	//	}
	//	ImGui::SliderFloat("Slomo", &mUpdateSpeed, 0.f, 2.f);
	//	ImGui::Checkbox("Draw Shadowmap", &mDrawShadowmap);
	//	if (ImGui::InputInt("Shadowmap Resolution", &mShadowmapResolution)) {
	//		framebuffer_params Params;
	//		Params.mType = framebuffer_type::shadowmap;
	//		Params.mWidth = mShadowmapResolution;
	//		Params.mHeight = mShadowmapResolution;
	//		mDirectionalShadowmap.Reload(Params);
	//
	//		Params.mType = framebuffer_type::shadowmap_omni;
	//		mPointShadowmap.Reload(Params);
	//	}
	//	if (ImGui::CollapsingHeader("Cubes")) {
	//		ImGui::InputFloat("Mat shininess", &mCubeMaterial.mShininess);
	//		if (ImGui::Button("Add Cube")) {
	//			mStaticCubes.Emplace(glm::mat4(1.f));
	//		}
	//
	//		if (mStaticCubes.Size() > 0) {
	//			ImGui::InputInt("Cube", &mCubeEditIndex);
	//			mCubeEditIndex = mCubeEditIndex % mStaticCubes.Size();
	//			ImGui::InputFloat4("Column 4", glm::value_ptr(mStaticCubes[mCubeEditIndex][3]));
	//			ImGui::InputFloat4("Column 3", glm::value_ptr(mStaticCubes[mCubeEditIndex][2]));
	//			ImGui::InputFloat4("Column 2", glm::value_ptr(mStaticCubes[mCubeEditIndex][1]));
	//			ImGui::InputFloat4("Column 1", glm::value_ptr(mStaticCubes[mCubeEditIndex][0]));
	//		}
	//	}
	//	if (ImGui::CollapsingHeader("Lights")) {
	//		ImGui::InputInt("Light", &mLightEditIndex);
	//		mLightEditIndex = mLightEditIndex % Renderer.mSceneLights.Size();
	//		Renderer.mSceneLights[mLightEditIndex].UIControlPanel("");
	//	}
	return {};
}

game_update_result game::Step(
	float Time,
	float DeltaTime,
	const input& Input,
	const window_state& WindowState,
	const render_state& RenderState) {
	game_update_result Result;
	Result.mUIData = UpdateUi();
	return Result;
}
