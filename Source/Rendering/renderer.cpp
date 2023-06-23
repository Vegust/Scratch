//
// Created by Vegust on 21.06.2023.
//

#include "renderer.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"

#include <iostream>

void GlClearError()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber)
{
	if (GLenum Error = glGetError())
	{
		std::cout << "OpenGl Error: (" << Error << "): " << FileName << ":" << LineNumber << ":"
				  << FunctionName << std::endl;
		return false;
	}
	return true;
}

void renderer::Draw(
	const vertex_array& VertexArray,
	const index_buffer& IndexBuffer,
	const shader& Shader)
{
	Shader.Bind();
	VertexArray.Bind();
	IndexBuffer.Bind();
	
	GL_CALL(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(IndexBuffer.GetCount()), GL_UNSIGNED_INT, nullptr));
}

void renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
