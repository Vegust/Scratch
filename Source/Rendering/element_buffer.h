//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

class element_buffer
{
private:
	uint32 RendererId{0};
	uint32 Count{0};

public:
	element_buffer() = default;
	~element_buffer();

	element_buffer(const element_buffer&) = delete;
	element_buffer& operator=(const element_buffer&) = delete;

	element_buffer(element_buffer&& InIndexBuffer);
	element_buffer& operator=(element_buffer&& InIndexBuffer);
	
	void SetData(const uint32* InData, uint32 InCount);

	void Bind() const;

	[[nodiscard]] uint32 GetCount() const
	{
		return Count;
	}
};
