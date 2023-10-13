#pragma once

#include "core_types.h"
#include "Asset/Model/model.h"
#include "Game/Entities/light.h"
#include "Game/Entities/model_instance.h"

struct world {
	dyn_array<light> mLights{};
	dyn_array<model_instance> mInstances{};
};