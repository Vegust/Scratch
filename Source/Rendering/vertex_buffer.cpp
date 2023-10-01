//
// Created by Vegust on 21.06.2023.
//

#include "vertex_buffer.h"

#include "renderer.h"

vertex_buffer::vertex_buffer(vertex_buffer&& InVertexBuffer) noexcept
{
	RendererId = InVertexBuffer.RendererId;
	InVertexBuffer.RendererId = 0;
}

vertex_buffer& vertex_buffer::operator=(vertex_buffer&& InVertexBuffer) noexcept
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
	RendererId = InVertexBuffer.RendererId;
	InVertexBuffer.RendererId = 0;
	return *this;
}

vertex_buffer::~vertex_buffer()
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
}

void vertex_buffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, RendererId);
}

void vertex_buffer::SetData(const void* InData, u32 InSize)
{
	if (RendererId == 0)
	{
		glGenBuffers(1, &RendererId);
	}
	glBindBuffer(GL_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(InSize), InData, GL_STATIC_DRAW);
}
