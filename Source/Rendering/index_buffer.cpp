//
// Created by Vegust on 21.06.2023.
//

#include "element_buffer.h"
#include "renderer.h"

element_buffer::element_buffer(element_buffer&& InIndexBuffer)
{
	RendererId = InIndexBuffer.RendererId;
	InIndexBuffer.RendererId = 0;
}

element_buffer& element_buffer::operator=(element_buffer&& InIndexBuffer)
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
	RendererId = InIndexBuffer.RendererId;
	InIndexBuffer.RendererId = 0;
	return *this;
}

element_buffer::~element_buffer()
{
	if (RendererId != 0)
	{
		glDeleteBuffers(1, &RendererId);
	}
}

void element_buffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
}

void element_buffer::SetData(const uint32* InData, uint32 InCount)
{
	if (RendererId == 0)
	{
		glGenBuffers(1, &RendererId);
	}
	Count = InCount;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(Count * sizeof(uint32)), InData, GL_STATIC_DRAW);
}
