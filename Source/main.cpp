//
// Created by Vegust on 17.06.2023.
//

#include "core_types.h"
SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Hello World", nullptr, nullptr);
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
		 0.5f, -0.5f, 1.f, 0.f,
		-0.5f, -0.5f, 0.f, 0.f,
		 0.5f,	0.5f, 1.f, 1.f,
		-0.5f,	0.5f, 0.f, 1.f
		// clang-format on
	};
	std::array<uint32, NumTriangles* 3> Indices = {0, 1, 2, 1, 2, 3};
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	{
		constexpr float AspectRatio = static_cast<float>(WindowWidth)/WindowHeight;
		glm::mat4 ProjectionMatrix = glm::ortho(-AspectRatio, AspectRatio, -1.f, 1.f, -1.f, 1.f);
		
		vertex_array VertexArray{};
		vertex_buffer VertexBuffer{Positions.data(), NumVertices * SizeOfVertex};
		vertex_buffer_layout VertexLayout{};
		VertexLayout.Push<float>(2);
		VertexLayout.Push<float>(2);
		VertexArray.AddBuffer(VertexBuffer, VertexLayout);

		index_buffer IndexBuffer{Indices.data(), Indices.size()};

		shader Shader{"Resources/Shaders/Basic.shader"};
		Shader.Bind();

		texture Texture{"Resources/Textures/OpenGL_Logo.png"};
		Texture.Bind();
		Shader.SetUniform1i("u_Texture", 0);
		Shader.SetUniformMat4f("u_MVP", ProjectionMatrix);
		
		renderer Renderer;

		while (!glfwWindowShouldClose(window))
		{
			Shader.Bind();
			Renderer.Clear();
			Renderer.Draw(VertexArray, IndexBuffer, Shader);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
