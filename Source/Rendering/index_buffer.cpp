//
// Created by Vegust on 21.06.2023.
//

#include "index_buffer.h"

#include "renderer.h"

index_buffer::index_buffer(index_buffer&& InIndexBuffer)
{
	RendererId = InIndexBuffer.RendererId;
	InIndexBuffer.RendererId = 0;
}

index_buffer& index_buffer::operator=(index_buffer&& InIndexBuffer)
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
	RendererId = InIndexBuffer.RendererId;
	InIndexBuffer.RendererId = 0;
	return *this;
}

index_buffer::~index_buffer()
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
}

void index_buffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
}

void index_buffer::SetData(const uint32* InData, uint32 InCount)
{
	if (RendererId == 0)
	{
		glGenBuffers(1, &RendererId);
	}
	Count = InCount;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint32), InData, GL_STATIC_DRAW);
}
