//
// Created by Vegust on 25.06.2023.
//

#include "camera.h"

#include "core_types.h"

SCRATCH_DISABLE_WARNINGS_BEGIN()
#include "GLFW/glfw3.h"
SCRATCH_DISABLE_WARNINGS_END()

void camera::ProcessInput(struct GLFWwindow* Window, float DeltaTime)
{
	const float Speed = DeltaTime * MovementSpeed;
	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += Speed * Direction;
	}
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position -= Speed * Direction;
	}
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position -= glm::normalize(glm::cross(Direction, UpVector)) * Speed;
	}
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += glm::normalize(glm::cross(Direction, UpVector)) * Speed;
	}
}

void camera::OnMouseMoved(struct GLFWwindow* Window, double XPosDelta, double YPosDelta)
{
	Yaw += static_cast<float>(XPosDelta) * Sensitivity;
	Pitch += static_cast<float>(-YPosDelta) * Sensitivity;

	if (Pitch > 89.0f)
	{
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f)
	{
		Pitch = -89.0f;
	}

	glm::vec3 NewDirection{};
	NewDirection.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	NewDirection.y = sin(glm::radians(Pitch));
	NewDirection.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Direction = glm::normalize(NewDirection);
}

void camera::OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta)
{
	FoV -= static_cast<float>(YDelta) * 5.f;
	if (FoV < 1.0f)
		FoV = 1.0f;
    if (FoV > 179.0f)
		FoV = 179.0f; 
}
