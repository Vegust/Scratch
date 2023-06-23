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

#include "Rendering/index_buffer.h"
#include "Rendering/renderer.h"
#include "Rendering/shader.h"
#include "Rendering/texture.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer.h"
#include "Rendering/vertex_buffer_layout.h"

#include "TestScenes/test_scene.h"
#include "TestScenes/test_clear_color.h"
#include "TestScenes/test_texture.h"

#include <array>

int main()
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr uint32 WindowWidth = 1920;
	constexpr uint32 WindowHeight = 1080;
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	SCRATCH_DISABLE_WARNINGS_BEGIN()
	assert(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)));
	SCRATCH_DISABLE_WARNINGS_END()

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
		
		test_scene* CurrentTestScene = nullptr;
		test_menu TestMenu{CurrentTestScene};
		CurrentTestScene = &TestMenu;
		
		TestMenu.RegisterTest<test_clear_color>("Clear Color");
		TestMenu.RegisterTest<test_texture>("Texture");

		while (!glfwWindowShouldClose(window))
		{
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glClearColor(0.2f, 0.2f, 0.2f, 1.f);
			renderer::Clear();
			
			if (CurrentTestScene)
			{
				CurrentTestScene->OnUpdate();
				CurrentTestScene->OnRender();
				ImGui::Begin("Test");
				if (CurrentTestScene != &TestMenu && ImGui::Button("<-"))
				{
					delete CurrentTestScene;
					CurrentTestScene = &TestMenu;
				}
				CurrentTestScene->OnIMGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
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
