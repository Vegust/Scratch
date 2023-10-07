#include "core_types.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"

#include <GLFW/glfw3.h>

#include "Rendering/OldRender/renderer.h"
#include "TestScenes/test_3d_texture.h"
#include "TestScenes/test_clear_color.h"
#include "TestScenes/test_scene.h"
#include "TestScenes/test_texture.h"

#include <array>

static test_scene*& GetCurrentScene() {
	[[clang::no_destroy]] static test_scene* Scene = nullptr;
	return Scene;
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
	glViewport(0, 0, NewWidth, NewHeight);
	renderer::Get().mAspectRatio = static_cast<float>(NewWidth) / static_cast<float>(NewHeight);
	renderer::Get().mCurrentHeight = NewHeight;
	renderer::Get().mCurrentWidth = NewWidth;
	if (GetCurrentScene()) {
		GetCurrentScene()->OnScreenSizeChanged(NewWidth, NewHeight);
	}
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
	static double LastXPos{XPos};
	static double LastYPos{YPos};

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!renderer::Get().mCustomCamera.expired()) {
				auto CameraHandle = renderer::Get().mCustomCamera.lock();
				CameraHandle->OnMouseMoved(Window, XPos - LastXPos, YPos - LastYPos);
			}
		}
	}

	LastXPos = XPos;
	LastYPos = YPos;
}

static void OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta) {
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!renderer::Get().mCustomCamera.expired()) {
				auto CameraHandle = renderer::Get().mCustomCamera.lock();
				CameraHandle->OnMouseScroll(Window, XDelta, YDelta);
			}
		}
	}
}

static void ProcessInput(GLFWwindow* Window, float DeltaTime) {
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Window, true);
		}
		if (GetCurrentScene()) {
			if (!renderer::Get().mCustomCamera.expired()) {
				if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				} else {
					glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}

				auto CameraHandle = renderer::Get().mCustomCamera.lock();
				CameraHandle->ProcessInput(Window, DeltaTime);
			}
			GetCurrentScene()->ProcessInput(Window, DeltaTime);
		}
	}
}

int main() {
	glfwInit();

	constexpr u32 WindowWidth = 1000;
	constexpr u32 WindowHeight = 800;

	renderer::Get().mCurrentHeight = WindowHeight;
	renderer::Get().mCurrentWidth = WindowWidth;
	renderer::Get().mAspectRatio =
		static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	if (!Window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);
	
	auto GLLoadingResult = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	CHECK(GLLoadingResult);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(Window, OnWindowResize);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);

	bool bVSync = true;
	bool bOldVSync = bVSync;

	bool bFreeCamera = false;
	bool bOldFreeCamera = bFreeCamera;

	std::shared_ptr<camera> FreeCamera{};
	std::weak_ptr<camera> ReplacedCamera{};

	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();

		glfwSetCursorPosCallback(Window, OnMouseMoved);
		glfwSetScrollCallback(Window, OnMouseScroll);
		ImGui_ImplGlfw_InitForOpenGL(Window, true);

		ImGui_ImplOpenGL3_Init("#version 460");

		test_menu TestMenu{GetCurrentScene()};
		GetCurrentScene() = &TestMenu;

		renderer::Get().Init();

		double LastTime = glfwGetTime();
		while (!glfwWindowShouldClose(Window)) {
			const double NewTime = glfwGetTime();
			const double DeltaTime = NewTime - LastTime;
			LastTime = NewTime;

			ProcessInput(Window, static_cast<float>(DeltaTime));

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0.2f, 0.2f, 0.2f, 1.f);
			renderer::Clear();

			if (GetCurrentScene()) {
				GetCurrentScene()->OnUpdate(static_cast<float>(DeltaTime));
				GetCurrentScene()->OnRender(renderer::Get());
				ImGui::Begin(
					"Debug",
					nullptr,
					ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowFontScale(1.5f);
				ImGui::Text("Frame time %.4f ms", 1000.0 * DeltaTime);
				ImGui::Text("%.1f FPS", static_cast<double>(io.Framerate));
				{
					ImGui::Checkbox("VSync", &bVSync);
					if (bOldVSync != bVSync) {
						glfwSwapInterval(bVSync ? 1 : 0);
					}
					bOldVSync = bVSync;
				}
				if (GetCurrentScene() != &TestMenu) {
					renderer::Get().UIRendererControls();
					ImGui::Checkbox("Free Camera", &bFreeCamera);
					if (bOldFreeCamera != bFreeCamera) {
						if (bFreeCamera) {
							ReplacedCamera = renderer::Get().mCustomCamera;
							FreeCamera = std::make_shared<camera>();
							renderer::Get().mCustomCamera = FreeCamera;
							if (!ReplacedCamera.expired()) {
								auto ReplacedHandle = ReplacedCamera.lock();
								FreeCamera->Direction = ReplacedHandle->Direction;
								FreeCamera->UpVector = ReplacedHandle->UpVector;
								FreeCamera->Position = ReplacedHandle->Position;
								FreeCamera->FoV = ReplacedHandle->FoV;
								FreeCamera->MovementSpeed = ReplacedHandle->MovementSpeed;
								FreeCamera->Pitch = ReplacedHandle->Pitch;
								FreeCamera->Yaw = ReplacedHandle->Yaw;
								FreeCamera->Sensitivity = ReplacedHandle->Sensitivity;
							} else {
								FreeCamera->Direction = renderer::Get().mCameraDirection;
								FreeCamera->UpVector = renderer::Get().mCameraUpVector;
								FreeCamera->Position = renderer::Get().mCameraPosition;
								FreeCamera->FoV = renderer::Get().mFoV;
							}
						} else {
							renderer::Get().mCustomCamera = ReplacedCamera;
							FreeCamera = nullptr;
						}
					}
					bOldFreeCamera = bFreeCamera;
				}

				if (GetCurrentScene() != &TestMenu && ImGui::Button("<-")) {
					delete GetCurrentScene();
					renderer::Get().mCustomCamera = ReplacedCamera;
					FreeCamera = nullptr;
					renderer::Get().ResetCamera();
					renderer::Get().mSceneLights.Clear();
					bFreeCamera = false;
					bOldFreeCamera = false;
					GetCurrentScene() = &TestMenu;
				}
				GetCurrentScene()->OnIMGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(Window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
