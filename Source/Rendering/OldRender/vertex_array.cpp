#include "vertex_array.h"

#include "index_buffer.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

vertex_array::vertex_array(vertex_array&& VertexArray) noexcept {
	RendererId = VertexArray.RendererId;
	IndexBufferSize = VertexArray.IndexBufferSize;
	InstanceCount = VertexArray.InstanceCount;
	VertexArray.InstanceCount = 1;
	VertexArray.RendererId = 0;
	VertexArray.IndexBufferSize = 0;
}

vertex_array& vertex_array::operator=(vertex_array&& VertexArray) noexcept{
	if (RendererId != 0) {
		glDeleteVertexArrays(1, &RendererId);
	}
	RendererId = VertexArray.RendererId;
	IndexBufferSize = VertexArray.IndexBufferSize;
	InstanceCount = VertexArray.InstanceCount;
	VertexArray.InstanceCount = 1;
	VertexArray.RendererId = 0;
	VertexArray.IndexBufferSize = 0;
	return *this;
}

vertex_array::~vertex_array() {
	if (RendererId != 0) {
		glDeleteVertexArrays(1, &RendererId);
	}
}

void vertex_array::SetData(
	vertex_buffer&& InVertexBuffer,
	index_buffer&& InIndexBuffer,
	const vertex_buffer_layout& InLayout) {
	if (RendererId == 0) {
		glGenVertexArrays(1, &RendererId);
	}
	Bind();
	InVertexBuffer.Bind();
	const auto& Attributes = InLayout.GetAttributes();
	u64 Offset = 0;
	for (u32 AttrIndex = 0; AttrIndex < InLayout.GetSize(); ++AttrIndex) {
		const auto& Attribute = Attributes[AttrIndex];
		glEnableVertexAttribArray(AttrIndex);
		glVertexAttribPointer(
			AttrIndex,
			static_cast<GLint>(Attribute.Count),
			Attribute.Type,
			Attribute.Normalized,
			static_cast<GLsizei>(InLayout.GetStride()),
			reinterpret_cast<const void*>(Offset));
		Offset += Attribute.Count * vertex_buffer_attribute::GetSizeOfType(Attribute.Type);
	}
	InIndexBuffer.Bind();
	IndexBufferSize = InIndexBuffer.GetCount();
	VertexBuffer = std::move(InVertexBuffer);
	IndexBuffer = std::move(InIndexBuffer);
}

void vertex_array::Bind() const {
	glBindVertexArray(RendererId);
}

void vertex_array::SetData(vertex_buffer&& InVertexBuffer, const vertex_buffer_layout& InLayout) {
	if (RendererId == 0) {
		glGenVertexArrays(1, &RendererId);
	}
	Bind();
	InVertexBuffer.Bind();
	const auto& Attributes = InLayout.GetAttributes();
	u64 Offset = 0;
	for (u32 AttrIndex = 0; AttrIndex < InLayout.GetSize(); ++AttrIndex) {
		const auto& Attribute = Attributes[AttrIndex];
		glEnableVertexAttribArray(AttrIndex);
		glVertexAttribPointer(
			AttrIndex,
			static_cast<GLint>(Attribute.Count),
			Attribute.Type,
			Attribute.Normalized,
			static_cast<GLsizei>(InLayout.GetStride()),
			reinterpret_cast<const void*>(Offset));
		Offset += Attribute.Count * vertex_buffer_attribute::GetSizeOfType(Attribute.Type);
	}
	IndexBufferSize = InVertexBuffer.GetCount();
	VertexBuffer = std::move(InVertexBuffer);
}
