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
#include "cubemap.h"
#include "framebuffer.h"
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

enum class view_mode : uint8
{
	lit = 0,
	unlit = 1,
	wireframe = 2,
	depth = 3
};

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
	void InitScreenQuadVAO();
	void InitSkyboxVAO();
	void InitDefaultShaders();
	
	void Draw(
		const vertex_array& VertexArray,
		const element_buffer& IndexBuffer,
		const shader& Shader,
		glm::mat4 Transform) const;
	void DrawCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;
	void DrawCubes(const phong_material& Material, const std::vector<glm::mat4>& Transforms) const;
	void DrawNormalCubes(const shader& Shader, const std::vector<glm::mat4>& Transforms) const;
	void Draw2(const vertex_array& VertexArray, const element_buffer& ElementBuffer, const phong_material& Material, const glm::mat4& Transform) const;
	void DrawFrameBuffer(const framebuffer& Framebuffer, bool bDepth = false);
	void DrawSkybox(const cubemap& Skybox);

	void ResetCamera()
	{
		CameraPosition = glm::vec3{0.f, 0.f, 0.f};
		CameraDirection = glm::vec3{0.f,0.f,-1.f};
		CameraUpVector = glm::vec3{0.f,1.f,0.f};
	}

	glm::mat4 CalcMVPForTransform(const glm::mat4& Transform) const;

	float AspectRatio = 1.f;
	uint32 CurrentHeight;
	uint32 CurrentWidth;
	
	std::weak_ptr<camera> CustomCamera{};
	
	float FoV = 60.f;
	glm::vec3 CameraPosition = glm::vec3{0.f, 0.f, 0.f};
	glm::vec3 CameraDirection = glm::vec3{0.f,0.f,-1.f};
	glm::vec3 CameraUpVector = glm::vec3{0.f,1.f,0.f};
	
	vertex_array ScreenQuadVAO{};
	vertex_buffer ScreenQuadBVO{};
		
	vertex_array CubeVAO{};
	vertex_buffer CubeVBO{};
	
	vertex_array NormalCubeVAO{};
	vertex_buffer NormalCubeVBO{};
	
	vertex_array SkyboxVAO{};
	vertex_buffer SkyboxVBO{};
	int32 SkyboxTextureSlot = cubemap::CubemapSlot;
	
	shader PostProcessShader{};
	
	shader SkyboxShader{};
	shader PhongShader{};
	shader OutlineShader{};
	shader NormalsShader{};
	shader* ActiveShader = &PhongShader;
	void SetActiveShader(shader* NewActiveShader) { ActiveShader = NewActiveShader; }
	
	std::vector<light> SceneLights{};

	void UIRendererControls()
	{
		UIViewModeControl();
		UIPostProcessControl();
	}
	
	void UIViewModeControl();
	void ChangeViewMode(view_mode NewViewMode);
	view_mode ViewMode = view_mode::lit;
	int32 DrawElementsMode = GL_TRIANGLES;
	void UIPostProcessControl();
	bool bGrayscale = false;
	bool bNormals = false;
	float GammaCorrection = 2.2f;
	
	bool bInstanced = false;
};
