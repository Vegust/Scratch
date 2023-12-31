//
// Created by Vegust on 21.06.2023.
//

#include "index_buffer.h"
#include "old_rebderer.h"

index_buffer::index_buffer(index_buffer&& InIndexBuffer)
{
	RendererId = InIndexBuffer.RendererId;
	Count = InIndexBuffer.Count;
	InIndexBuffer.RendererId = 0;
	InIndexBuffer.Count = 0;
}

index_buffer& index_buffer::operator=(index_buffer&& InIndexBuffer)
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
	RendererId = InIndexBuffer.RendererId;
	Count = InIndexBuffer.Count;
	InIndexBuffer.RendererId = 0;
	InIndexBuffer.Count = 0;
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

void index_buffer::SetData(const u32* InData, u32 InCount)
{
	if (RendererId == 0)
	{
		glGenBuffers(1, &RendererId);
	}
	Count = InCount;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(Count * sizeof(u32)), InData, GL_STATIC_DRAW);
}
