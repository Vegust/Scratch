//
// Created by Vegust on 21.06.2023.
//

#include "vertex_array.h"

#include "vertex_buffer.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"

vertex_array::vertex_array()
{
	glGenVertexArrays(1, &RendererId);
	glBindVertexArray(RendererId);
}

vertex_array::~vertex_array()
{
	glDeleteVertexArrays(1, &RendererId);
}

void vertex_array::AddBuffer(const vertex_buffer& VertexBuffer, const vertex_buffer_layout& Layout)
{
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
	const index_buffer& IndexBuffer,
	const vertex_buffer_layout& Layout)
{
	AddBuffer(VertexBuffer, Layout);
	IndexBuffer.Bind();
}

void vertex_array::Bind() const
{
	glBindVertexArray(RendererId);
}

void vertex_array::Unbind() const
{
	glBindVertexArray(0);
}
