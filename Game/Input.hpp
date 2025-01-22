#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

class App;

class InputManager {
public:
	InputManager();

	~InputManager();

	void Input();

	glm::ivec2 mousePos;

	bool mLastkey_Relevant{ false };

	SDL_Keycode mLastKey_Keycode{ SDLK_UNKNOWN };

	SDL_Scancode mLastKey_Scancode{ SDL_SCANCODE_UNKNOWN };
	
	SDL_Scancode mMOVE_UP_Bind{ SDL_SCANCODE_W };
	SDL_Scancode mMOVE_DOWN_Bind{ SDL_SCANCODE_S };
	SDL_Scancode mMOVE_LEFT_Bind{ SDL_SCANCODE_A };
	SDL_Scancode mMOVE_RIGHT_Bind{ SDL_SCANCODE_D };
	SDL_Scancode mJUMP_Bind{ SDL_SCANCODE_SPACE };
	SDL_Scancode mDUCK_Bind{ SDL_SCANCODE_LSHIFT };

private:
	App& app();

	App* app_;

	bool LClick{ false };
};