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

#include <array>

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr uint32 WindowWidth = 1920;
	constexpr uint32 WindowHeight = 1080;
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Scratch", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	SCRATCH_DISABLE_WARNINGS_BEGIN()
	assert(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)));
	SCRATCH_DISABLE_WARNINGS_END()

	constexpr uint32 NumTriangles = 2;
	constexpr uint32 NumVertices = 4;
	constexpr uint32 SizeOfVertex = 4 * sizeof(float);
	std::array<float, NumVertices* SizeOfVertex> Positions = {
		// clang-format off
		 0.8f, -0.8f, 1.f, 0.f,
		-0.8f, -0.8f, 0.f, 0.f,
		 0.8f,	0.8f, 1.f, 1.f,
		-0.8f,	0.8f, 0.f, 1.f
		// clang-format on
	};
	std::array<uint32, NumTriangles* 3> Indices = {0, 1, 2, 1, 2, 3};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{
		constexpr float InitialAspectRatio = static_cast<float>(WindowWidth) / WindowHeight;
		glm::mat4 ProjectionMatrix = glm::ortho(-InitialAspectRatio, InitialAspectRatio, -1.f, 1.f, -1.f, 1.f);
		glm::mat4 ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(-0.5, 0.f, 0.f));
		glm::mat4 ModelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(0.5, 0.f, 0.f));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		vertex_array VertexArray{};
		vertex_buffer VertexBuffer{Positions.data(), NumVertices * SizeOfVertex};
		vertex_buffer_layout VertexLayout{};
		VertexLayout.Push<float>(2);
		VertexLayout.Push<float>(2);
		VertexArray.AddBuffer(VertexBuffer, VertexLayout);

		index_buffer IndexBuffer{Indices.data(), Indices.size()};

		texture Texture{"Resources/Textures/SlostestPhone.jpg"};
		Texture.Bind();

		shader Shader{"Resources/Shaders/Basic.shader"};
		Shader.Bind();
		Shader.SetUniform1i("u_Texture", 0);
		Shader.SetUniformMat4f("u_MVP", MVP);

		renderer Renderer;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	 // Enable Gamepad Controls
		io.IniFilename = nullptr;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		auto PicTrans = glm::vec3(0.5, 0.f, 0.f);
		
		while (!glfwWindowShouldClose(window))
		{
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			
			float AspectRatio = static_cast<float>(display_w) / static_cast<float>(display_h);
			ProjectionMatrix = glm::ortho(-AspectRatio, AspectRatio, -1.f, 1.f, -1.f, 1.f);
			ViewMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(-0.5, 0.f, 0.f));
			ModelMatrix = glm::translate(glm::mat4{1.0f}, PicTrans);
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			Shader.Bind();
			Shader.SetUniformMat4f("u_MVP", MVP);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			
			ImGui::SliderFloat3("float", &PicTrans.x, -1.f, 1.f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / static_cast<double>(io.Framerate), static_cast<double>(io.Framerate));

			Renderer.Clear();
			Renderer.Draw(VertexArray, IndexBuffer, Shader);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
