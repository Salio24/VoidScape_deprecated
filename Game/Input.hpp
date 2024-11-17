#pragma once
#include <SDL3/SDL.h>

class App;

class InputManager {
	App& app();

	App* app_;

public:
	InputManager();

	~InputManager();

	void Input();

};