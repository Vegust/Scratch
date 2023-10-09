#pragma once

class application;

struct ui {
	void Init(application* App);
	void OnNewFrame();
	void StartDebugWindow(float DeltaTime);
	void EndDebugWindow();
	void Render();
};