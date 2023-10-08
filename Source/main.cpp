#include "core_types.h"
#include "Application/application.h"

//static void OnWindowResize(GLFWwindow* Window, int NewWidth, int NewHeight) {
//	glViewport(0, 0, NewWidth, NewHeight);
//	renderer::Get().mAspectRatio = static_cast<float>(NewWidth) / static_cast<float>(NewHeight);
//	renderer::Get().mCurrentHeight = NewHeight;
//	renderer::Get().mCurrentWidth = NewWidth;
//	if (auto* TestMap = reinterpret_cast<test_shadowmaps*>(glfwGetWindowUserPointer(Window))) {
//		TestMap->OnScreenSizeChanged(NewWidth, NewHeight);
//	}
//}
//
//static void OnMouseMoved(GLFWwindow* Window, double XPos, double YPos) {
//	static double LastXPos{XPos};
//	static double LastYPos{YPos};
//
//	ImGuiIO& io = ImGui::GetIO();
//	if (!io.WantCaptureMouse) {
//		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//			if (!renderer::Get().mCustomCamera.expired()) {
//				auto CameraHandle = renderer::Get().mCustomCamera.lock();
//				CameraHandle->OnMouseMoved(Window, XPos - LastXPos, YPos - LastYPos);
//			}
//		}
//	}
//
//	LastXPos = XPos;
//	LastYPos = YPos;
//}
//
//static void OnMouseScroll(struct GLFWwindow* Window, double XDelta, double YDelta) {
//	ImGuiIO& io = ImGui::GetIO();
//	if (!io.WantCaptureMouse) {
//		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//			if (!renderer::Get().mCustomCamera.expired()) {
//				auto CameraHandle = renderer::Get().mCustomCamera.lock();
//				CameraHandle->OnMouseScroll(Window, XDelta, YDelta);
//			}
//		}
//	}
//}

int main() {
	application App{};
	App.Run();
	return 0;
}
