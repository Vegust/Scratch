#include "mesh.h"

#include "Rendering/OldRender/old_rebderer.h"

void mesh::Init() {
	vertex_buffer VertexBuffer{};
	index_buffer ElementBuffer{};
	VertexBuffer.SetData(mVertices.Data(), static_cast<u32>(mVertices.Size()) * static_cast<u32>(sizeof(vertex)), mVertices.Size());
	ElementBuffer.SetData(mIndices.Data(), static_cast<u32>(mIndices.Size()));
	mVertexArray.SetData(std::move(VertexBuffer), std::move(ElementBuffer), vertex::GetLayout());
}
