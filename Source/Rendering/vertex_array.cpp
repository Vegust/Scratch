//
// Created by Vegust on 21.06.2023.
//

#include "vertex_array.h"

#include "element_buffer.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

vertex_array::vertex_array(vertex_array&& InVertexArray)
{
	RendererId = InVertexArray.RendererId;
	ElementBufferSize = InVertexArray.ElementBufferSize;
	InVertexArray.RendererId = 0;
	InVertexArray.ElementBufferSize = 0;
}

vertex_array& vertex_array::operator=(vertex_array&& InVertexArray)
{
	if (RendererId != 0)
	{
		glDeleteVertexArrays(1, &RendererId);
	}
	RendererId = InVertexArray.RendererId;
	ElementBufferSize = InVertexArray.ElementBufferSize;
	InVertexArray.RendererId = 0;
	InVertexArray.ElementBufferSize = 0;
	return *this;
}

vertex_array::~vertex_array()
{
	if (RendererId != 0)
	{
		glDeleteVertexArrays(1, &RendererId);
	}
}

void vertex_array::AddBuffer(const vertex_buffer& VertexBuffer, const vertex_buffer_layout& Layout)
{
	if (RendererId == 0)
	{
		glGenVertexArrays(1, &RendererId);
	}
	Bind();
	VertexBuffer.Bind();
	const auto& Attributes = Layout.GetAttributes();
	uint32 Offset = 0;
	for (uint32 AttrIndex = 0; AttrIndex < Attributes.size(); ++AttrIndex)
	{
		const auto& Attribute = Attributes[AttrIndex];
		glEnableVertexAttribArray(AttrIndex);
		glVertexAttribPointer(
			AttrIndex,
			static_cast<GLint>(Attribute.Count),
			Attribute.Type,
			Attribute.Normalized,
			static_cast<GLsizei>(Layout.GetStride()),
			reinterpret_cast<const void*>(Offset));
		Offset += Attribute.Count * vertex_buffer_attribute::GetSizeOfType(Attribute.Type);
	}
}

void vertex_array::AddBuffer(
	const vertex_buffer& VertexBuffer,
	const element_buffer& IndexBuffer,
	const vertex_buffer_layout& Layout)
{
	AddBuffer(VertexBuffer, Layout);
	IndexBuffer.Bind();
	ElementBufferSize = IndexBuffer.GetCount();
}

void vertex_array::Bind() const
{
	glBindVertexArray(RendererId);
}
