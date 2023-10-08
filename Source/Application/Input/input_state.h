#pragma once

#include "core_types.h"
#include "input_types.h"

struct input_state {
protected:
	key_state mKeyStates[static_cast<u64>(input_key::max)]{key_state::up};
	mouse_data mMouseData{};

public:
	key_state GetKeyState(input_key Key) {
		return mKeyStates[static_cast<u64>(Key)];
	}
	
	const mouse_data& GetMouseData() {
		return mMouseData;
	}
	
	void SetKeyState(input_key Key, key_state State) {
		
	}
};