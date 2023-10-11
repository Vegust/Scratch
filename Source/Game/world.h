#pragma once

#include "core_types.h"
#include "SceneObjects/camera.h"
#include "SceneObjects/model.h"
#include "SceneObjects/light.h"
#include "Containers/array.h"

struct world {
	camera mCamera{};
	dyn_array<light> mLights{};
	dyn_array<model> mModels{};
};