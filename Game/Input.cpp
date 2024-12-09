#include "Input.hpp"
#include "App.hpp"

InputManager::InputManager() : app_(nullptr) {

}

InputManager::~InputManager() {

}

App& InputManager::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

void InputManager::Input() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			std::cout << "Goodbye!" << std::endl;
			app().mQuit = true;
		}
		else if (e.type == SDL_EVENT_MOUSE_MOTION) {
		}
		if (e.type == SDL_EVENT_KEY_UP) {
			switch (e.key.scancode) {
			case SDL_SCANCODE_SPACE:
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::SPACE)] = false;
				app().mMovementHandler.spacebarOneShot = true;
				break;
			case SDL_SCANCODE_A:
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = false;
				break;
			case SDL_SCANCODE_D:
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = false;
				break;
			case SDL_SCANCODE_LSHIFT: 
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::DUCK)] = false;
				app().mMovementHandler.duckOneShot = true;
				break;
			case SDL_SCANCODE_W:
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_UP)] = false;
				break;
			case SDL_SCANCODE_S:
				app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)] = false;
				break;

			default:
				break;
			}
		}
		const bool* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_R]) {
			app().LoadGame();
		}
		if (state[SDL_SCANCODE_SPACE]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::SPACE)] = true;
		}
		if (state[SDL_SCANCODE_W]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_UP)] = true;
		}
		if (state[SDL_SCANCODE_S]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)] = true;
		}
		if (state[SDL_SCANCODE_LSHIFT]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::DUCK)] = true;
		}
		if (state[SDL_SCANCODE_A]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = true;
		}
		if (state[SDL_SCANCODE_D]) {
			app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = true;
		}
		if (state[SDL_SCANCODE_ESCAPE]) {
			app().mQuit = true;
		}
	}
}