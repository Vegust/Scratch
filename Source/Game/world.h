#pragma once

#include "core_types.h"
#include "SceneObjects/model.h"
#include "SceneObjects/light.h"
#include "SceneObjects/model_instance.h"

struct world {
	dyn_array<light> mLights{};
	dyn_array<model_instance> mInstances{};
};