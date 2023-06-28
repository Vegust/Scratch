//
// Created by Vegust on 17.06.2023.
//

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"

#include <GLFW/glfw3.h>
SCRATCH_DISABLE_WARNINGS_END()

#include "Rendering/renderer.h"
#include "TestScenes/test_3d_texture.h"
#include "TestScenes/test_clear_color.h"
#include "TestScenes/test_scene.h"
#include "TestScenes/test_texture.h"

#include <array>
#include <iostream>

static test_scene*& GetCurrentScene()
{
	[[clang::no_destroy]] static test_scene* Scene = nullptr;
	return Scene;
}

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight)
{
	glViewport(0, 0, NewWidth, NewHeight);
	renderer::Get().AspectRatio = static_cast<float>(NewWidth) / static_cast<float>(NewHeight);
}

static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos)
{
	static double LastXPos{XPos};
	static double LastYPos{YPos};
	
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			if (!renderer::Get().CustomCamera.expired())
			{
				auto CameraHandle = renderer::Get().CustomCamera.lock();
				CameraHandle->OnMouseMoved(Window, XPos - LastXPos, YPos - LastYPos);
			}
		}
	}
	
	LastXPos = XPos;
	LastYPos = YPos;
}

static void OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
	{
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			if (!renderer::Get().CustomCamera.expired())
			{
				auto CameraHandle = renderer::Get().CustomCamera.lock();
				CameraHandle->OnMouseScroll(Window, XDelta, YDelta);
			}
		}
	}
}

static void ProcessInput(GLFWwindow* Window, float DeltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse)
	{
		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(Window, true);
		}
		if (GetCurrentScene())
		{
			if (!renderer::Get().CustomCamera.expired())
			{
				if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				{
					glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
				}
				else
				{
					glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
				}
				
				auto CameraHandle = renderer::Get().CustomCamera.lock();
				CameraHandle->ProcessInput(Window, DeltaTime);
			}
			GetCurrentScene()->ProcessInput(Window, DeltaTime);
		}
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr uint32 WindowWidth = 1920;
	constexpr uint32 WindowHeight = 1080;
	renderer::Get().AspectRatio =
		static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	if (!Window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(Window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(Window, OnWindowResize);

	SCRATCH_DISABLE_WARNINGS_BEGIN()
	auto GLLoadingResult = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	assert(GLLoadingResult);
	SCRATCH_DISABLE_WARNINGS_END()

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

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
		while (!glfwWindowShouldClose(Window))
		{
			const double NewTime = glfwGetTime();
			const double DeltaTime = NewTime - LastTime;
			LastTime = NewTime;

			ProcessInput(Window, static_cast<float>(DeltaTime));

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0.2f, 0.2f, 0.2f, 1.f);
			renderer::Clear();

			if (GetCurrentScene())
			{
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
					if (bOldVSync != bVSync)
					{
						glfwSwapInterval(bVSync ? 1 : 0);
					}
					bOldVSync = bVSync;
				}
				if (GetCurrentScene() != &TestMenu)
				{
					ImGui::Checkbox("Free Camera", &bFreeCamera);
					if (bOldFreeCamera != bFreeCamera)
					{
						if (bFreeCamera)
						{
							ReplacedCamera = renderer::Get().CustomCamera;
							FreeCamera = std::make_shared<camera>();
							renderer::Get().CustomCamera = FreeCamera;
							if (!ReplacedCamera.expired())
							{
								auto ReplacedHandle = ReplacedCamera.lock();
								FreeCamera->Direction = ReplacedHandle->Direction;
								FreeCamera->UpVector = ReplacedHandle->UpVector;
								FreeCamera->Position = ReplacedHandle->Position;
								FreeCamera->FoV = ReplacedHandle->FoV;
								FreeCamera->MovementSpeed = ReplacedHandle->MovementSpeed;
								FreeCamera->Pitch = ReplacedHandle->Pitch;
								FreeCamera->Yaw = ReplacedHandle->Yaw;
								FreeCamera->Sensitivity = ReplacedHandle->Sensitivity;
							}
							else
							{
								FreeCamera->Direction = renderer::Get().CameraDirection;
								FreeCamera->UpVector = renderer::Get().CameraUpVector;
								FreeCamera->Position = renderer::Get().CameraPosition;
								FreeCamera->FoV = renderer::Get().FoV;
							}
						}
						else
						{
							renderer::Get().CustomCamera = ReplacedCamera;
							FreeCamera = nullptr;
						}
					}
					bOldFreeCamera = bFreeCamera;
				}
				
				if (GetCurrentScene() != &TestMenu && ImGui::Button("<-"))
				{
					delete GetCurrentScene();
					renderer::Get().CustomCamera = ReplacedCamera;
					FreeCamera = nullptr;
					renderer::Get().ResetCamera();
					renderer::Get().SceneLights.clear();
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
