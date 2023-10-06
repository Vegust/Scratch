#pragma once

#include "Containers/tag.h"
#include "resource.h"

struct resource_storage {
	ref<resource> Load(tag Name);
	ref<resource> Create();
};
