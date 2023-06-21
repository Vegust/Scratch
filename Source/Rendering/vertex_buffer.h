//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class vertex_buffer
{
private:
	uint32 RendererId{};

public:
	vertex_buffer(const void* InData, uint32 InSize);
	~vertex_buffer();

	void Bind() const;
	void Unbind() const;
};
