//
// Created by Vegust on 17.06.2023.
//

#include "core_types.h"
#include <cassert>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include <GLFW/glfw3.h>
SCRATCH_DISABLE_WARNINGS_END()

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	
	// TODO: maybe you can rewrite it without clang warnings
	SCRATCH_DISABLE_WARNINGS_BEGIN()
	assert(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)));
	SCRATCH_DISABLE_WARNINGS_END()

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glBegin(GL_TRIANGLES);
		//glVertex2f(-0.5f,-0.5f);
		//glVertex2f(-0.0f,0.5f);
		//glVertex2f(0.5f,-0.5f);
		//glEnd();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
