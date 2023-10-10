#pragma once

#include "test_shadowmaps.h"
#include "world.h"

class renderer;
class application;

class game {
public:

	// TODO refactor
	test_shadowmaps mTestMap{};

	world mWorld{};

	void Init(renderer& Renderer);
	void Update(float DeltaTime, renderer& Renderer, application* App);
	void HandleInput(float DeltaTime, application* App);
};