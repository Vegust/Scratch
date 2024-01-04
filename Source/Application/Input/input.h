#pragma once

#include "basic.h"
#include "input_types.h"
#include "Containers/array.h"

struct frame_input_state {
	array<key_state, static_cast<u32>(input_key::max)> KeyStates{};
	mouse_state MouseState{};

	frame_input_state& operator=(const frame_input_state& Other) {
		MouseState = Other.MouseState;
		memcpy(&KeyStates, &Other.KeyStates, sizeof(KeyStates));
		return *this;
	}
};

struct input {
	frame_input_state LastFrame{};
	frame_input_state ThisFrame{};

	[[nodiscard]] FORCEINLINE bool MouseMovedThisFrame() const {
		return ThisFrame.MouseState.Pos != LastFrame.MouseState.Pos;
	}

	[[nodiscard]] FORCEINLINE bool MouseScrolledThisFrame() const {
		return ThisFrame.MouseState.Scroll != vec2{0.f};
	}

	[[nodiscard]] FORCEINLINE vec2 GetMousePos() const {
		return ThisFrame.MouseState.Pos;
	}

	[[nodiscard]] FORCEINLINE vec2 GetMousePosDelta() const {
		return ThisFrame.MouseState.Pos - LastFrame.MouseState.Pos;
	}

	[[nodiscard]] FORCEINLINE vec2 GetScroll() const {
		return ThisFrame.MouseState.Scroll;
	}

	[[nodiscard]] FORCEINLINE bool Pressed(input_key Key) const {
		return ThisFrame.KeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}

	[[nodiscard]] FORCEINLINE bool Released(input_key Key) const {
		return ThisFrame.KeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool PressedThisFrame(input_key Key) const {
		return Pressed(Key) && LastFrame.KeyStates[static_cast<u64>(Key)] == key_state::released;
	}

	[[nodiscard]] FORCEINLINE bool ReleasedThisFrame(input_key Key) const {
		return Released(Key) && LastFrame.KeyStates[static_cast<u64>(Key)] == key_state::pressed;
	}
};