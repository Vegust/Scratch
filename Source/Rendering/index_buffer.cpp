//
// Created by Vegust on 21.06.2023.
//

#include "index_buffer.h"

#include "renderer.h"

index_buffer::index_buffer(const uint32* InData, uint32 InCount) : Count(InCount)
{
	glGenBuffers(1, &RendererId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint32), InData, GL_STATIC_DRAW);
}

index_buffer::~index_buffer()
{
	glDeleteBuffers(1, &RendererId);
}

void index_buffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
}

void index_buffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
