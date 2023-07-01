//
// Created by Vegust on 30.06.2023.
//

#include "test_instancing.h"

#include "imgui.h"

REGISTER_TEST_SCENE(test_instancing, "08 Instancing")

test_instancing::test_instancing()
{
	SunShader.Compile("Resources/Shaders/BasicLight.shader");
	InstancedPhongShader.Compile("Resources/Shaders/BasicShadedInstanced.shader");

	Model.Load("Resources/Models/Rock/rock.obj");

	std::vector<std::string_view> Paths = {
		"Left_Tex.jpg",
		"Right_Tex.jpg",
		"Up_Tex.jpg",
		"Down_Tex.jpg",
		"Front_Tex.jpg",
		"Back_Tex.jpg"};
	Skybox.Load("Resources/Textures/Skybox_Stars", Paths);

	auto& Light = renderer::Get().SceneLights.emplace_back();
	Light.Type = light_type::point;
	Light.AttenuationRadius = 1000.f;
	Light.Ambient = {0.f, 0.f, 0.f};

	Camera = std::make_shared<camera>();
	Camera->Position = glm::vec3{0.f, 1.f, 10.f};
	renderer::Get().CustomCamera = Camera;

	RecalculateAsteroidTransforms();
}

void test_instancing::OnRender(renderer& Renderer)
{
	test_scene::OnRender(Renderer);

	glClearColor(0.1f, 0.1f, 0.05f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Sun
	const glm::mat4 View = Camera->GetViewTransform();
	std::vector<glm::mat4> LightTransforms;
	glm::mat4 LightTransform = glm::scale(
		glm::translate(glm::mat4{1.0f}, renderer::Get().SceneLights[0].Position),
		glm::vec3(1.f, 1.0f, 1.0f));
	LightTransforms.push_back(LightTransform);
	SunShader.Bind();
	SunShader.SetUniform("u_Light", renderer::Get().SceneLights[0], View);
	Renderer.DrawNormalCubes(SunShader, LightTransforms);

	// Asteroids
	if (bInstancing)
	{
		Renderer.SetActiveShader(&InstancedPhongShader);
		Renderer.bInstanced = true;
		Model.Draw(Renderer, glm::mat4(1.f));	 // Transforms stored in bound instanced buffer
		Renderer.bInstanced = false;
		Renderer.SetActiveShader(&Renderer.PhongShader);
	}
	else
	{
		for (const auto& Transform : AsteroidTransforms)
		{
			Model.Draw(Renderer, Transform);
		}
	}
	
	// Stars
	Renderer.DrawSkybox(Skybox);
}

void test_instancing::OnIMGuiRender()
{
	test_scene::OnIMGuiRender();

	const int32 OldAsteroidCount = AsteroidCount;
	const float OldInnerRadius = InnerRadius;
	const float OldOuterRadius = OuterRadius;
	const float OldVerticalSpread = VerticalSpread;

	ImGui::Checkbox("Instancing", &bInstancing);
	ImGui::InputInt("Asteroid count", &AsteroidCount);
	ImGui::InputFloat("Inner Radius", &InnerRadius);
	ImGui::InputFloat("Outer Radius", &OuterRadius);
	ImGui::InputFloat("Vertical Spread", &VerticalSpread);

	if (OldAsteroidCount != AsteroidCount || OldInnerRadius != InnerRadius ||
		OldOuterRadius != OuterRadius || OldVerticalSpread != VerticalSpread)
	{
		RecalculateAsteroidTransforms();
	}

	renderer::Get().SceneLights[0].UIControlPanel("");
}

void test_instancing::RecalculateAsteroidTransforms()
{
	AsteroidTransforms.clear();
	AsteroidTransforms.reserve(AsteroidCount);
	for (int32 i = 0; i < AsteroidCount; ++i)
	{
		using namespace glm;
		const float VerticalOffset = VerticalSpread * (((rand() % 10000) / 5000.f) - 1.f);
		const float RotationLocal = pi<float>() * 2 * ((rand() % 10000) / 10000.f);
		const float Radius =
			InnerRadius + ((rand() % 10000) / 10000.f) * (OuterRadius - InnerRadius);
		const float Rotation = pi<float>() * 2 * ((rand() % 10000) / 10000.f);
		AsteroidTransforms.push_back(scale(
			rotate(
				translate(
					rotate(
						translate(mat4(1.f), vec3(0.f, VerticalOffset, 0.f)),
						Rotation,
						vec3(0.f, 1.f, 0.f)),
					vec3(Radius, 0.f, 0.f)),
				RotationLocal,
				vec3(1.f, 0.3f, 0.1f)),
			vec3(0.1f, 0.1f, 0.1f)));
	}

	// Instanced buffer
	InstancesBuffer = {};	 // Reset
	InstancesBuffer.SetData(
		AsteroidTransforms.data(), AsteroidTransforms.size() * sizeof(glm::mat4));
	InstancesBuffer.Bind();
	for (auto& Mesh : Model.Meshes)
	{
		Mesh.VertexArray.Bind();
		Mesh.VertexArray.InstanceCount = AsteroidTransforms.size();
		
		std::size_t Vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3); 
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*)0);
		glEnableVertexAttribArray(4); 
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*)(1 * Vec4Size));
		glEnableVertexAttribArray(5); 
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*)(2 * Vec4Size));
		glEnableVertexAttribArray(6); 
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * Vec4Size, (void*)(3 * Vec4Size));
		
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		
		glBindVertexArray(0);
	}
}
