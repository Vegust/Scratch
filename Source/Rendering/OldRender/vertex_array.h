#pragma once

#include "core_types.h"

class element_buffer;
class vertex_buffer;
struct vertex_buffer_layout;

class vertex_array {
public:
	u32 mRendererId{0};
	u32 mElementBufferSize{0};

	u32 mInstanceCount = 1;

	vertex_array() = default;
	~vertex_array();

	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	vertex_array(vertex_array&& InVertexArray);
	vertex_array& operator=(vertex_array&& InVertexArray);

	void AddBuffer(const vertex_buffer& VertexBuffer, const vertex_buffer_layout& Layout);
	void AddBuffer(
		const vertex_buffer& VertexBuffer,
		const element_buffer& IndexBuffer,
		const vertex_buffer_layout& Layout);

	void Bind() const;
};
