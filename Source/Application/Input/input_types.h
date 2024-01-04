#pragma once

#include "basic.h"

enum class key_state : u8 {
	released,
	pressed
};

enum class input_key : u32 {
	none = 0,
	mouse_left,
	mouse_right,
	mouse_middle,
	keyboard_enter,
	keyboard_shift,
	keyboard_control,
	keyboard_alt,
	keyboard_space,
	keyboard_escape,
	keyboard_tab,
	keyboard_backspace,
	keyboard_insert,
	keyboard_delete,
	keyboard_right,
	keyboard_left,
	keyboard_down,
	keyboard_up,
	keyboard_0,
	keyboard_1,
	keyboard_2,
	keyboard_3,
	keyboard_4,
	keyboard_5,
	keyboard_6,
	keyboard_7,
	keyboard_8,
	keyboard_9,
	keyboard_a,
	keyboard_b,
	keyboard_c,
	keyboard_d,
	keyboard_e,
	keyboard_f,
	keyboard_g,
	keyboard_h,
	keyboard_i,
	keyboard_j,
	keyboard_k,
	keyboard_l,
	keyboard_m,
	keyboard_n,
	keyboard_o,
	keyboard_p,
	keyboard_q,
	keyboard_r,
	keyboard_s,
	keyboard_t,
	keyboard_u,
	keyboard_v,
	keyboard_w,
	keyboard_x,
	keyboard_y,
	keyboard_z,
	keyboard_f1,
	keyboard_f2,
	keyboard_f3,
	keyboard_f4,
	keyboard_f5,
	keyboard_f6,
	keyboard_f7,
	keyboard_f8,
	keyboard_f9,
	keyboard_f10,
	keyboard_f11,
	keyboard_f12,
	max
};

struct mouse_state {
	vec2 Pos{};
	vec2 Scroll{};
};