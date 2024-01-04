#pragma once

#include "basic.h"

class vertex_buffer {
private:
	u32 mRendererId{0};
	u32 mCount{0};

public:
	vertex_buffer() = default;
	~vertex_buffer();

	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;
	vertex_buffer(vertex_buffer&& InVertexBuffer) noexcept;
	vertex_buffer& operator=(vertex_buffer&& InVertexBuffer) noexcept;

	[[nodiscard]] u32 GetCount() const { return mCount; }

	void SetData(const void* Data, u32 Size, u32 Count);

	void Bind() const;
};
