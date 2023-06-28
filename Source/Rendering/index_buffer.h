//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class index_buffer
{
private:
	uint32 RendererId{0};
	uint32 Count{0};

public:
	index_buffer() = default;
	~index_buffer();
	
	index_buffer(const index_buffer&) = delete;
	index_buffer& operator=(const index_buffer&) = delete;
	
	index_buffer(index_buffer&& InIndexBuffer);
	index_buffer& operator=(index_buffer&& InIndexBuffer);
	
	void SetData(const uint32* InData, uint32 InCount);

	void Bind() const;

	[[nodiscard]] uint32 GetCount() const
	{
		return Count;
	}
};
