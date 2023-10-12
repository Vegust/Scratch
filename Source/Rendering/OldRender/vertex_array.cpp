#include "vertex_array.h"

#include "index_buffer.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

vertex_array::vertex_array(vertex_array&& VertexArray) noexcept {
	mRendererId = VertexArray.mRendererId;
	mElementBufferSize = VertexArray.mElementBufferSize;
	mInstanceCount = VertexArray.mInstanceCount;
	VertexArray.mInstanceCount = 1;
	VertexArray.mRendererId = 0;
	VertexArray.mElementBufferSize = 0;
}

vertex_array& vertex_array::operator=(vertex_array&& VertexArray) noexcept{
	if (mRendererId != 0) {
		glDeleteVertexArrays(1, &mRendererId);
	}
	mRendererId = VertexArray.mRendererId;
	mElementBufferSize = VertexArray.mElementBufferSize;
	mInstanceCount = VertexArray.mInstanceCount;
	VertexArray.mInstanceCount = 1;
	VertexArray.mRendererId = 0;
	VertexArray.mElementBufferSize = 0;
	return *this;
}

vertex_array::~vertex_array() {
	if (mRendererId != 0) {
		glDeleteVertexArrays(1, &mRendererId);
	}
}

void vertex_array::SetData(
	vertex_buffer&& VertexBuffer,
	index_buffer&& IndexBuffer,
	const vertex_buffer_layout& Layout) {
	if (mRendererId == 0) {
		glGenVertexArrays(1, &mRendererId);
	}
	Bind();
	VertexBuffer.Bind();
	const auto& Attributes = Layout.GetAttributes();
	u64 Offset = 0;
	for (u32 AttrIndex = 0; AttrIndex < Layout.mSize; ++AttrIndex) {
		const auto& Attribute = Attributes[AttrIndex];
		glEnableVertexAttribArray(AttrIndex);
		glVertexAttribPointer(
			AttrIndex,
			static_cast<GLint>(Attribute.mCount),
			Attribute.mType,
			Attribute.mNormalized,
			static_cast<GLsizei>(Layout.GetStride()),
			reinterpret_cast<const void*>(Offset));
		Offset += Attribute.mCount * vertex_buffer_attribute::GetSizeOfType(Attribute.mType);
	}
	IndexBuffer.Bind();
	mElementBufferSize = IndexBuffer.GetCount();
	mVertexBuffer = std::move(VertexBuffer);
	mElementBuffer = std::move(IndexBuffer);
}

void vertex_array::Bind() const {
	glBindVertexArray(mRendererId);
}

void vertex_array::SetData(vertex_buffer&& VertexBuffer, const vertex_buffer_layout& Layout) {
	if (mRendererId == 0) {
		glGenVertexArrays(1, &mRendererId);
	}
	Bind();
	VertexBuffer.Bind();
	const auto& Attributes = Layout.GetAttributes();
	u64 Offset = 0;
	for (u32 AttrIndex = 0; AttrIndex < Layout.mSize; ++AttrIndex) {
		const auto& Attribute = Attributes[AttrIndex];
		glEnableVertexAttribArray(AttrIndex);
		glVertexAttribPointer(
			AttrIndex,
			static_cast<GLint>(Attribute.mCount),
			Attribute.mType,
			Attribute.mNormalized,
			static_cast<GLsizei>(Layout.GetStride()),
			reinterpret_cast<const void*>(Offset));
		Offset += Attribute.mCount * vertex_buffer_attribute::GetSizeOfType(Attribute.mType);
	}
	mElementBufferSize = VertexBuffer.GetCount();
	mVertexBuffer = std::move(VertexBuffer);
}
