#pragma once

#include "basic.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

struct camera {
	glm::vec3 mPosition{0.f, 0.f, 0.f};
	glm::vec3 mDirection{0.f, 0.f, -1.f};
	glm::vec3 mUpVector{0.f, 1.f, 0.f};
	float mFoV{60.f};
	float mMovementSpeed{5.f};
	float mSensitivity{0.1f};
	float mYaw{-90.f};
	float mPitch{0.f};

	[[nodiscard]] FORCEINLINE float GetFoV() const {
		return mFoV;
	}

	[[nodiscard]] FORCEINLINE glm::mat4 GetViewTransform() const {
		return glm::lookAt(mPosition, mPosition + mDirection, mUpVector);
	}
};
