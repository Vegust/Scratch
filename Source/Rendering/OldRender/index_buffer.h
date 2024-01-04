//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "basic.h"

class index_buffer {
private:
	u32 RendererId{0};
	u32 Count{0};

public:
	index_buffer() = default;
	~index_buffer();

	index_buffer(const index_buffer&) = delete;
	index_buffer& operator=(const index_buffer&) = delete;

	index_buffer(index_buffer&& InIndexBuffer);
	index_buffer& operator=(index_buffer&& InIndexBuffer);
	
	void SetData(const u32* InData, u32 InCount);

	void Bind() const;

	[[nodiscard]] u32 GetCount() const
	{
		return Count;
	}
};
