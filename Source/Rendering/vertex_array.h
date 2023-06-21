//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"
class vertex_buffer;
class vertex_buffer_layout;

class vertex_array
{
private:
	uint32 RendererId{0};

public:
	vertex_array();
	~vertex_array();

	void AddBuffer(const vertex_buffer& VertexBuffer, const vertex_buffer_layout& Layout);

	void Bind() const;
	void Unbind() const;
};
