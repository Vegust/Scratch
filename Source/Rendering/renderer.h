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
#include "shader.h"
#include "vertex_buffer.h"
SCRATCH_DISABLE_WARNINGS_END()

#define GL_CALL(x)  \
	GlClearError(); \
	x;              \
	GlLogCall(#x, __FILE__, __LINE__);

class shader;

void GlClearError();
bool GlLogCall(const char* FunctionName, const char* FileName, int LineNumber);

class renderer
{
public:
	static void Clear();
	static void Draw(
		const vertex_array& VertexArray,
		const element_buffer& IndexBuffer,
		const shader& Shader);
	
	static renderer& Get()
	{
		static renderer Renderer;
		return Renderer;
	}

	void Init();
	void InitCubeVAO();
	void InitNormalCubeVAO();
	void InitDefaultShaders();
	
	void Draw(
		const vertex_array& VertexArray,
		const element_buffer& IndexBuffer,
		const shader& Shader,
		glm::mat4 Transform) const;
	void DrawCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;
	void DrawNormalCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;
	
	void DrawPhong(const vertex_array& VertexArray, const phong_material& Material, const glm::mat4& Transform) const;

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
	
	vertex_array CubeVAO{};
	vertex_buffer CubeVBO{};
	
	vertex_array NormalCubeVAO{};
	vertex_buffer NormalCubeVBO{};
	
	shader PhongShader{};
	std::vector<light> SceneLights{};
};
