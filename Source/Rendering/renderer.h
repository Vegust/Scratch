//
// Created by Vegust on 21.06.2023.
//

#pragma once

#include "SceneObjects/camera.h"
#include "core_types.h"
#include "vertex_array.h"

#include <memory>
#include <vector>

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
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
	static void Clear();
	static void Draw(
		const vertex_array& VertexArray,
		const index_buffer& IndexBuffer,
		const shader& Shader);
	
	static renderer& Get()
	{
		[[clang::no_destroy]] static renderer Renderer;
		return Renderer;
	}

	void Init();
	void InitCubeVAO();
	void InitNormalCubeVAO();
	
	void Draw(
		const vertex_array& VertexArray,
		const index_buffer& IndexBuffer,
		const shader& Shader,
		glm::mat4 Transform) const;
	void DrawCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;
	void DrawNormalCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;

	void ResetCamera()
	{
		CameraPosition = glm::vec3{0.f, 0.f, 0.f};
		CameraDirection = glm::vec3{0.f,0.f,-1.f};
		CameraUpVector = glm::vec3{0.f,1.f,0.f};
	}

	glm::mat4 CalcMVPForTransform(const glm::mat4& Transform) const;

	float AspectRatio = 1.f;
	
	std::weak_ptr<camera> CustomCamera{};
	
	float FoV = 60.f;
	glm::vec3 CameraPosition = glm::vec3{0.f, 0.f, 0.f};
	glm::vec3 CameraDirection = glm::vec3{0.f,0.f,-1.f};
	glm::vec3 CameraUpVector = glm::vec3{0.f,1.f,0.f};
	
	std::unique_ptr<vertex_array> CubeVAO{nullptr};
	
	std::unique_ptr<vertex_array> NormalCubeVAO{nullptr};
};
