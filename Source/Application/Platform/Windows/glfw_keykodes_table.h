#pragma once

#include "Application/Input/input_types.h"
#include <GLFW/glfw3.h>

constexpr static array<input_key, GLFW_KEY_LAST> CreateMapping() {
	array<input_key, GLFW_KEY_LAST> Result{};
	Result[GLFW_KEY_KP_ENTER] = input_key::keyboard_enter;
	Result[GLFW_KEY_RIGHT_SHIFT] = input_key::keyboard_shift;
	Result[GLFW_KEY_LEFT_SHIFT] = input_key::keyboard_shift;
	Result[GLFW_KEY_RIGHT_CONTROL] = input_key::keyboard_control;
	Result[GLFW_KEY_LEFT_CONTROL] = input_key::keyboard_control;
	Result[GLFW_KEY_RIGHT_ALT] = input_key::keyboard_alt;
	Result[GLFW_KEY_LEFT_ALT] = input_key::keyboard_alt;
	Result[GLFW_KEY_SPACE] = input_key::keyboard_space;
	Result[GLFW_KEY_ESCAPE] = input_key::keyboard_escape;
	Result[GLFW_KEY_TAB] = input_key::keyboard_tab;
	Result[GLFW_KEY_BACKSPACE] = input_key::keyboard_backspace;
	Result[GLFW_KEY_INSERT] = input_key::keyboard_insert;
	Result[GLFW_KEY_DELETE] = input_key::keyboard_delete;
	Result[GLFW_KEY_RIGHT] = input_key::keyboard_right;
	Result[GLFW_KEY_LEFT] = input_key::keyboard_left;
	Result[GLFW_KEY_DOWN] = input_key::keyboard_down;
	Result[GLFW_KEY_UP] = input_key::keyboard_up;
	Result[GLFW_KEY_0] = input_key::keyboard_0;
	Result[GLFW_KEY_1] = input_key::keyboard_1;
	Result[GLFW_KEY_2] = input_key::keyboard_2;
	Result[GLFW_KEY_3] = input_key::keyboard_3;
	Result[GLFW_KEY_4] = input_key::keyboard_4;
	Result[GLFW_KEY_5] = input_key::keyboard_5;
	Result[GLFW_KEY_6] = input_key::keyboard_6;
	Result[GLFW_KEY_7] = input_key::keyboard_7;
	Result[GLFW_KEY_8] = input_key::keyboard_8;
	Result[GLFW_KEY_9] = input_key::keyboard_9;
	Result[GLFW_KEY_A] = input_key::keyboard_a;
	Result[GLFW_KEY_B] = input_key::keyboard_b;
	Result[GLFW_KEY_C] = input_key::keyboard_c;
	Result[GLFW_KEY_D] = input_key::keyboard_d;
	Result[GLFW_KEY_E] = input_key::keyboard_e;
	Result[GLFW_KEY_F] = input_key::keyboard_f;
	Result[GLFW_KEY_G] = input_key::keyboard_g;
	Result[GLFW_KEY_H] = input_key::keyboard_h;
	Result[GLFW_KEY_I] = input_key::keyboard_i;
	Result[GLFW_KEY_J] = input_key::keyboard_j;
	Result[GLFW_KEY_K] = input_key::keyboard_k;
	Result[GLFW_KEY_L] = input_key::keyboard_l;
	Result[GLFW_KEY_M] = input_key::keyboard_m;
	Result[GLFW_KEY_N] = input_key::keyboard_n;
	Result[GLFW_KEY_O] = input_key::keyboard_o;
	Result[GLFW_KEY_P] = input_key::keyboard_p;
	Result[GLFW_KEY_Q] = input_key::keyboard_q;
	Result[GLFW_KEY_R] = input_key::keyboard_r;
	Result[GLFW_KEY_S] = input_key::keyboard_s;
	Result[GLFW_KEY_T] = input_key::keyboard_t;
	Result[GLFW_KEY_U] = input_key::keyboard_u;
	Result[GLFW_KEY_V] = input_key::keyboard_v;
	Result[GLFW_KEY_W] = input_key::keyboard_w;
	Result[GLFW_KEY_X] = input_key::keyboard_x;
	Result[GLFW_KEY_Y] = input_key::keyboard_y;
	Result[GLFW_KEY_Z] = input_key::keyboard_z;
	Result[GLFW_KEY_F1] = input_key::keyboard_f1;
	Result[GLFW_KEY_F2] = input_key::keyboard_f2;
	Result[GLFW_KEY_F3] = input_key::keyboard_f3;
	Result[GLFW_KEY_F4] = input_key::keyboard_f4;
	Result[GLFW_KEY_F5] = input_key::keyboard_f5;
	Result[GLFW_KEY_F6] = input_key::keyboard_f6;
	Result[GLFW_KEY_F7] = input_key::keyboard_f7;
	Result[GLFW_KEY_F8] = input_key::keyboard_f8;
	Result[GLFW_KEY_F9] = input_key::keyboard_f9;
	Result[GLFW_KEY_F10] = input_key::keyboard_f10;
	Result[GLFW_KEY_F11] = input_key::keyboard_f11;
	Result[GLFW_KEY_F12] = input_key::keyboard_f12;
	return Result;
}

constexpr inline array<input_key, GLFW_KEY_LAST> GlfwButtonToInputKey = CreateMapping();
constexpr inline array<input_key, 3> GlfwMouseToInputKey = {
	input_key::mouse_left,
	input_key::mouse_right,
	input_key::mouse_middle};