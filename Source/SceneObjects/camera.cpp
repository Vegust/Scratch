#include "camera.h"
#include "Application/Input/input.h"

void camera::ProcessInput(input_state& Input, float DeltaTime) {
	const float Speed = DeltaTime * MovementSpeed;
	if (Input.Pressed(input_key::keyboard_w)) {
		Position += Speed * Direction;
	}
	if (Input.Pressed(input_key::keyboard_s)) {
		Position -= Speed * Direction;
	}
	if (Input.Pressed(input_key::keyboard_a)) {
		Position -= glm::normalize(glm::cross(Direction, UpVector)) * Speed;
	}
	if (Input.Pressed(input_key::keyboard_d)) {
		Position += glm::normalize(glm::cross(Direction, UpVector)) * Speed;
	}
	if (Input.MouseMovedThisFrame()) {
		Yaw += static_cast<float>(Input.GetMouseState().mPosFrameDeltaX) * Sensitivity;
		Pitch += static_cast<float>(-Input.GetMouseState().mPosFrameDeltaY) * Sensitivity;

		if (Pitch > 89.0f) {
			Pitch = 89.0f;
		}
		if (Pitch < -89.0f) {
			Pitch = -89.0f;
		}

		glm::vec3 NewDirection{};
		NewDirection.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		NewDirection.y = sin(glm::radians(Pitch));
		NewDirection.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Direction = glm::normalize(NewDirection);
	}
	if (Input.MouseScrolledThisFrame()) {
		FoV -= static_cast<float>(Input.GetMouseState().mScrollDeltaY) * 5.f;
		if (FoV < 1.0f) {
			FoV = 1.0f;
		}
		if (FoV > 359.0f) {
			FoV = 359.0f;
		}
	}
}
