//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class element_buffer
{
private:
	u32 RendererId{0};
	u32 Count{0};

public:
	element_buffer() = default;
	~element_buffer();

	element_buffer(const element_buffer&) = delete;
	element_buffer& operator=(const element_buffer&) = delete;

	element_buffer(element_buffer&& InIndexBuffer);
	element_buffer& operator=(element_buffer&& InIndexBuffer);
	
	void SetData(const u32* InData, u32 InCount);

	void Bind() const;

	[[nodiscard]] u32 GetCount() const
	{
		return Count;
	}
};
