#pragma once

#include "basic.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

struct vertex_buffer_layout;

class vertex_array {
private:
	u32 RendererId{0};
	index IndexBufferSize{0};
	index InstanceCount = 1;

	vertex_buffer VertexBuffer;
	index_buffer IndexBuffer;

public:
	vertex_array() = default;
	~vertex_array();

	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	vertex_array(vertex_array&& InVertexArray) noexcept;
	vertex_array& operator=(vertex_array&& InVertexArray) noexcept;
	
	[[nodiscard]] FORCEINLINE index GetIndexBufferSize() const {
		return IndexBufferSize;
	}

	void SetData(vertex_buffer&& VertexBuffer, index_buffer&& InElementBuffer, const vertex_buffer_layout& Layout);
	void SetData(vertex_buffer&& VertexBuffer, const vertex_buffer_layout& Layout);

	void Bind() const;
};
