#include "mesh.h"

#include "Rendering/OldRender/renderer.h"

void mesh::Init() {
	mVertexBuffer.SetData(
		mVertices.Data(), static_cast<u32>(mVertices.Size()) * static_cast<u32>(sizeof(vertex)));
	mElementBuffer.SetData(mIndices.Data(), static_cast<u32>(mIndices.Size()));
	mVertexArray.AddBuffer(mVertexBuffer, mElementBuffer, vertex::GetLayout());
}

void mesh::Draw(const renderer& Renderer, const glm::mat4& Transform) {
	Renderer.Draw2(mVertexArray, mElementBuffer, mMaterial, Transform);
}
