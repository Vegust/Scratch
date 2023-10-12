#pragma once

#include "core_types.h"
#include "ref.h"

struct model_instance {
	ref<model> mModel;
	mat4 mTransform{};
};