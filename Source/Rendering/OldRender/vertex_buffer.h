//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class vertex_buffer
{
private:
	u32 RendererId{0};

public:
	vertex_buffer() = default;
	~vertex_buffer();
	
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer(vertex_buffer&& InVertexBuffer) noexcept;
	vertex_buffer& operator=(vertex_buffer&& InVertexBuffer) noexcept;
	
	void SetData(const void* InData, u32 InSize);

	void Bind() const;
};
