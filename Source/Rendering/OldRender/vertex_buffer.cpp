#include "vertex_buffer.h"

#include "old_rebderer.h"

vertex_buffer::vertex_buffer(vertex_buffer&& InVertexBuffer) noexcept {
	mRendererId = InVertexBuffer.mRendererId;
	mCount = InVertexBuffer.mCount;
	InVertexBuffer.mRendererId = 0;
	InVertexBuffer.mCount = 0;
}

vertex_buffer& vertex_buffer::operator=(vertex_buffer&& InVertexBuffer) noexcept {
	if (mRendererId != 0) {
		glDeleteBuffers(1, &mRendererId);
	}
	mRendererId = InVertexBuffer.mRendererId;
	InVertexBuffer.mRendererId = 0;
	return *this;
}

vertex_buffer::~vertex_buffer() {
	if (mRendererId != 0) {
		glDeleteBuffers(1, &mRendererId);
	}
}

void vertex_buffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
}

void vertex_buffer::SetData(const void* Data, u32 Size, u32 Count) {
	if (mRendererId == 0) {
		glGenBuffers(1, &mRendererId);
	}
	mCount = Count;
	glBindBuffer(GL_ARRAY_BUFFER, mRendererId);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(Size), Data, GL_STATIC_DRAW);
}
