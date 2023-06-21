//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class index_buffer
{
private:
	uint32 RendererId{};
	uint32 Count{};

public:
	index_buffer(const uint32* InData, uint32 InCount);
	~index_buffer();

	void Bind() const;
	void Unbind() const;

	[[nodiscard]] uint32 GetCount() const
	{
		return Count;
	}
};
