//
// Created by Vegust on 27.06.2023.
//

#include "mesh.h"

#include "Rendering/renderer.h"

void mesh::Init()
{
	VertexBuffer.SetData(
		Vertices.data(),
		static_cast<uint32>(Vertices.size()) * static_cast<uint32>(sizeof(vertex)));
	ElementBuffer.SetData(Indices.data(), static_cast<uint32>(Indices.size()));
	VertexArray.AddBuffer(VertexBuffer, ElementBuffer, vertex::GetLayout());
}

void mesh::Draw(const renderer& Renderer, const glm::mat4& Transform)
{
	Renderer.Draw2(VertexArray, ElementBuffer, Material, Transform);
}
