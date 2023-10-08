#pragma once

enum class input_type {
	button_press,
	button_release,
	button_repeat,
	mouse_move,
	mouse_scroll
};

enum class key_state {
	up,
	pressed, // this frame
	down,
	released, // this frame
};

enum class input_key {
	none,
	mouse_left,
	mouse_right,
	mouse_middle,
	keyboard_esc,
	keyboard_w,
	keyboard_a,
	keyboard_s,
	keyboard_d,
	max
};

struct mouse_data {
	float mPosX{0.f};
	float mPosY{0.f};
	float mScrollDeltaX{0.f};
	float mScrollDeltaY{0.f};
};

struct input_action {
	input_type mType{input_type::button_press};
	input_key mKey{input_key::none};
	union {
		mouse_data mMouseData{};
	};
};