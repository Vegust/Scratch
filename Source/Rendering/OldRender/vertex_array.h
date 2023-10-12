#pragma once

#include "core_types.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

struct vertex_buffer_layout;

class vertex_array {
public:
	u32 mRendererId{0};
	u32 mElementBufferSize{0};
	u32 mInstanceCount = 1;

	vertex_buffer mVertexBuffer;
	index_buffer mElementBuffer;

	vertex_array() = default;
	~vertex_array();

	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	vertex_array(vertex_array&& InVertexArray) noexcept;
	vertex_array& operator=(vertex_array&& InVertexArray) noexcept;

	void SetData(vertex_buffer&& VertexBuffer, index_buffer&& IndexBuffer, const vertex_buffer_layout& Layout);
	void SetData(vertex_buffer&& VertexBuffer, const vertex_buffer_layout& Layout);

	void Bind() const;
};
