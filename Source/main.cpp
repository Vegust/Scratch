//
// Created by Vegust on 17.06.2023.
//

#include "Rendering/index_buffer.h"
#include "Rendering/renderer.h"
#include "Rendering/vertex_buffer.h"
#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "Rendering/shader.h"
#include "Rendering/vertex_array.h"
#include "Rendering/vertex_buffer_layout.h"
#include "glad/glad.h"

#include <GLFW/glfw3.h>
SCRATCH_DISABLE_WARNINGS_END()

#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

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

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
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
	constexpr uint32 SizeOfVertex = 2 * sizeof(float);
	constexpr uint32 FloatsPerTriangle = 6;
	std::array<float, FloatsPerTriangle* NumTriangles> Positions = {
		0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5, 0.5};
	std::array<uint32, NumTriangles* 3> Indices = {0, 1, 2, 1, 2, 3};
	
	{
		vertex_array VertexArray{};
		vertex_buffer VertexBuffer{Positions.data(), NumVertices * SizeOfVertex};
		vertex_buffer_layout VertexLayout{};
		VertexLayout.Push<float>(2);
		VertexArray.AddBuffer(VertexBuffer, VertexLayout);
		
		index_buffer IndexBuffer{Indices.data(), Indices.size()};

		shader Shader{"Resources/Shaders/Basic.shader"};
		Shader.Bind();
		Shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		
		renderer Renderer;

		float R = 0.f;
		float Increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			Shader.Bind();
			Shader.SetUniform4f("u_Color", R, 0.3f, 0.8f, 1.0f);
			
			Renderer.Clear();
			Renderer.Draw(VertexArray, IndexBuffer, Shader);

			if (R > 1.f)
			{
				Increment = -0.05f;
			}
			else if (R < 0.f)
			{
				Increment = 0.05f;
			}
			R += Increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
