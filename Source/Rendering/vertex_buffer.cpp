//
// Created by Vegust on 21.06.2023.
//

#include "vertex_buffer.h"

#include "renderer.h"

vertex_buffer::vertex_buffer(const void* InData, uint32 InSize)
{
	glGenBuffers(1, &RendererId);
	glBindBuffer(GL_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ARRAY_BUFFER, InSize, InData, GL_STATIC_DRAW);
}

vertex_buffer::~vertex_buffer()
{
	glDeleteBuffers(1, &RendererId);
}

void vertex_buffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, RendererId);
}

void vertex_buffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
