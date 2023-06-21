//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
SCRATCH_DISABLE_WARNINGS_END()

#define ASSERT(x) \
	if (!(x))     \
		__builtin_debugtrap();
#define GL_CALL(x)  \
	GlClearError(); \
	x;              \
	ASSERT(GlLogCall(#x, __FILE__, __LINE__));

class vertex_array;
class index_buffer;
class shader;

void GlClearError();
bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber);

class renderer
{
public:
	void Clear() const;
	void Draw(const vertex_array& VertexArray, const index_buffer& IndexBuffer, const shader& Shader) const;
};
