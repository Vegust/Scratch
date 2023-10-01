//
// Created by Vegust on 25.06.2023.
//

#pragma once

#include "core_types.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

struct camera
{
	glm::vec3 Position{0.f,0.f,0.f};
	glm::vec3 Direction{0.f,0.f,-1.f};
	glm::vec3 UpVector{0.f,1.f,0.f};
	float FoV{60.f};
	float MovementSpeed{5.f};
	float Sensitivity{0.1f};
	float Yaw{-90.f};
	float Pitch{0.f};
	
	void ProcessInput(struct GLFWwindow* Window, float DeltaTime);
	void OnMouseMoved(struct GLFWwindow* Window, double XPosDelta, double YPosDelta);
	void OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta);
	
	float GetFoV()
	{
		return FoV;
	}
	
	const glm::mat4 GetViewTransform()
	{
		return glm::lookAt(Position,Position+Direction,UpVector);
	}
};
