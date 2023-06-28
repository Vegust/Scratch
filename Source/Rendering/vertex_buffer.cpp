//
// Created by Vegust on 21.06.2023.
//

#include "vertex_buffer.h"

#include "renderer.h"

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

void vertex_buffer::SetData(const void* InData, uint32 InSize)
{
	if (RendererId == 0)
	{
		glGenBuffers(1, &RendererId);
	}
	glBindBuffer(GL_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ARRAY_BUFFER, InSize, InData, GL_STATIC_DRAW);
}
