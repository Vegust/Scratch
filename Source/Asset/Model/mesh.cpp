#include "mesh.h"

#include "Rendering/OldRender/old_rebderer.h"

void mesh::Init() {
	vertex_buffer VertexBuffer{};
	index_buffer ElementBuffer{};
	VertexBuffer.SetData(mVertices.GetData(), static_cast<u32>(mVertices.GetSize()) * static_cast<u32>(sizeof(vertex)), mVertices.GetSize());
	ElementBuffer.SetData(mIndices.GetData(), static_cast<u32>(mIndices.GetSize()));
	mVertexArray.SetData(std::move(VertexBuffer), std::move(ElementBuffer), vertex::GetLayout());
}
