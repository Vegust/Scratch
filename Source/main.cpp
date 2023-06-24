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
#include "TestScenes/test_clear_color.h"
#include "TestScenes/test_scene.h"
#include "TestScenes/test_texture.h"
#include "TestScenes/test_3d_texture.h"

#include <array>
#include <iostream>

[[clang::no_destroy]] static renderer Renderer;

static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight)
{
	glViewport(0, 0, NewWidth, NewHeight);
	Renderer.AspectRatio = static_cast<float>(NewWidth)/ static_cast<float>(NewHeight);
}

static void ProcessInput(GLFWwindow* Window)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard)
	{
		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(Window, true);
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
	Renderer.AspectRatio = static_cast<float>(WindowWidth)/ static_cast<float>(WindowHeight);
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
	assert(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)));
	SCRATCH_DISABLE_WARNINGS_END()

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
	bool bVSync = true;
	bool bOldVSync = bVSync;
	
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(Window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
		
		test_scene* CurrentTestScene = nullptr;
		test_menu TestMenu{CurrentTestScene};
		CurrentTestScene = &TestMenu;
		
		Renderer.Init();
		
		double LastTime = glfwGetTime();
		while (!glfwWindowShouldClose(Window))
		{
			const double NewTime = glfwGetTime();
			const double DeltaTime = NewTime - LastTime;
			LastTime = NewTime;
			
			ProcessInput(Window);
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0.2f, 0.2f, 0.2f, 1.f);
			renderer::Clear();
			
			if (CurrentTestScene)
			{
				CurrentTestScene->OnUpdate(static_cast<float>(DeltaTime));
				CurrentTestScene->OnRender(Renderer);
				ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowFontScale(1.5f);
				ImGui::Text("Frame time %.4f ms", 1000.0 * DeltaTime);
				ImGui::Text("%.1f FPS", static_cast<double>(io.Framerate));
				ImGui::Checkbox("VSync", &bVSync);
				if (bOldVSync != bVSync)
				{
					glfwSwapInterval(bVSync ? 1 : 0);
				}
				bOldVSync = bVSync;
				if (CurrentTestScene != &TestMenu && ImGui::Button("<-"))
				{
					delete CurrentTestScene;
					Renderer.ResetCamera();
					CurrentTestScene = &TestMenu;
				}
				CurrentTestScene->OnIMGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(Window);
			glfwPollEvents();
		}
		
		if (CurrentTestScene != &TestMenu)
		{
			delete CurrentTestScene;
		}
	}

	glfwTerminate();
	return 0;
}
