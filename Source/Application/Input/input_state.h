#pragma once

#include "core_types.h"
#include "input_types.h"
#include "Containers/array.h"

struct input_state {
	array<key_state, static_cast<u32>(input_key::max)> mLastFrameKeyStates{};
	array<key_state, static_cast<u32>(input_key::max)> mKeyStates{};
	mouse_data mMouseData{};

	void SaveFrameInput() {
		memcpy(&mLastFrameKeyStates, &mKeyStates, mKeyStates.Size() * sizeof(key_state));
		mMouseData.mScrollDeltaX = 0.f;
		mMouseData.mScrollDeltaY = 0.f;
		mMouseData.mPosFrameDeltaX = 0.f;
		mMouseData.mPosFrameDeltaY = 0.f;
	}

	[[nodiscard]] FORCEINLINE bool MouseMovedThisFrame() const {
		return mMouseData.mPosFrameDeltaX != 0.f || mMouseData.mPosFrameDeltaY != 0.f;
	}

	[[nodiscard]] FORCEINLINE bool MouseScrolledThisFrame() const {
		return mMouseData.mScrollDeltaX != 0.f || mMouseData.mScrollDeltaY != 0.f;
	}

	[[nodiscard]] FORCEINLINE bool Pressed(input_key Key) const {
		return mKeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}

	[[nodiscard]] FORCEINLINE bool Released(input_key Key) const {
		return mKeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool PressedThisFrame(input_key Key) const {
		return Pressed(Key) && mLastFrameKeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool ReleasedThisFrame(input_key Key) const {
		return Released(Key) && mLastFrameKeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}

	FORCEINLINE key_state GetKeyState(input_key Key) {
		return mKeyStates[static_cast<u64>(Key)];
	}

	FORCEINLINE void SetKeyState(input_key Key, key_state State) {
		mKeyStates[static_cast<u32>(Key)] = State;
	}

	FORCEINLINE mouse_data& GetMouseData() {
		return mMouseData;
	}
};