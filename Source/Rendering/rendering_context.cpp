#include "core_types.h"
#include "rendering_context.h"
#include "glad/glad.h"
#include "Application/Platform/window.h"

void rendering_context::Init(rendering_api Api, window& Window) {
	Window.SetContextCurrent();
	switch (Api) {
		case rendering_api::opengl:
			auto GLLoadingResult =
				gladLoadGLLoader(reinterpret_cast<GLADloadproc>(Window.GetApiLoadingFunction(Api)));
			CHECK(GLLoadingResult);
			break;
	}
}
