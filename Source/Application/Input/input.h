#pragma once

#include "core_types.h"
#include "input_types.h"
#include "Containers/array.h"

struct frame_input_state {
	array<key_state, static_cast<u32>(input_key::max)> mKeyStates{};
	mouse_state mMouseState{};

	frame_input_state& operator=(const frame_input_state& Other) {
		mMouseState = Other.mMouseState;
		memcpy(&mKeyStates, &Other.mKeyStates, sizeof(mKeyStates));
	}
};

struct input {
	frame_input_state mLastFrame{};
	frame_input_state mThisFrame{};

	[[nodiscard]] FORCEINLINE bool MouseMovedThisFrame() const {
		return mThisFrame.mMouseState.mPos != mLastFrame.mMouseState.mPos;
	}

	[[nodiscard]] FORCEINLINE bool MouseScrolledThisFrame() const {
		return mThisFrame.mMouseState.mScroll != vec2{0.f};
	}

	[[nodiscard]] FORCEINLINE vec2 GetMousePos() const {
		return mThisFrame.mMouseState.mPos;
	}

	[[nodiscard]] FORCEINLINE vec2 GetMousePosDelta() const {
		return mThisFrame.mMouseState.mPos - mLastFrame.mMouseState.mPos;
	}

	[[nodiscard]] FORCEINLINE vec2 GetScroll() const {
		return mThisFrame.mMouseState.mScroll;
	}

	[[nodiscard]] FORCEINLINE bool Pressed(input_key Key) const {
		return mThisFrame.mKeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}

	[[nodiscard]] FORCEINLINE bool Released(input_key Key) const {
		return mThisFrame.mKeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool PressedThisFrame(input_key Key) const {
		return Pressed(Key) && mLastFrame.mKeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool ReleasedThisFrame(input_key Key) const {
		return Released(Key) && mLastFrame.mKeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}
};