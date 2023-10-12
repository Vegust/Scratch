#include "dynamic_rhi.h"
#include "Rendering/rendering_types.h"
#include "Memory/memory.h"

#include "Rendering/Backend/OpenGL/opengl_rhi.h"

rhi* CreateRHI(rendering_api Api) {
	switch (Api) {
		case rendering_api::opengl:
			return StaticNew<opengl_rhi>();
	}
	return nullptr;
}