#include "Rendering/renderer.h"

void renderer::Init(u32 WindowWidth, u32 WindowHeight) {
	mRHI = CreateRHI(mRenderingApi);
	mRHI->Init();
	mOldRenderer.Init(WindowWidth, WindowHeight);
}

void renderer::RenderView() {

}
