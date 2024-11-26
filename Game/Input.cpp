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
	//static int mouseX = app().mWindowWidth / 2;
	//static int mouseY = app().mWindowHeight / 2;
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			std::cout << "Goodbye!" << std::endl;
			app().mQuit = true;
		}
		else if (e.type == SDL_EVENT_MOUSE_MOTION) {
			//std::cout << "mouseX: " << e.motion.x << " mouseY: " << e.motion.y << "" << std::endl;
			app().mCamera.SetMousePosstition(e.motion.x, e.motion.y);
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
		if (e.type == SDL_EVENT_KEY_DOWN) {
			switch (e.key.scancode) {
			case SDL_SCANCODE_SPACE:
				//app().mMovementHandler.KeyboadStates[static_cast<int>(MovementState::SPACE)] = true;
				break;
			default:
				break;
			}
		}
		const bool* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_UP]) {;
			app().mMovementHandler.GravityState = true;
		}
		if (state[SDL_SCANCODE_DOWN]) {
			app().mMovementHandler.GravityState = false;
		}
		if (state[SDL_SCANCODE_RIGHT]) {
			app().mActor.mPosition.x = 900.0f;
		}
		if (state[SDL_SCANCODE_LEFT]) {
			app().tmpbool1 = true;
			app().tmpbool2 = true;
		}
		//if (state[SDL_SCANCODE_LEFTBRACKET]) {
		//	gMesh1.mTranslate.m_uOffsetZ = +0.01f;
		//}
		//if (state[SDL_SCANCODE_RIGHTBRACKET]) {
		//	gMesh1.mTranslate.m_uOffsetZ = -0.01f;
		//}
		//if (state[SDL_SCANCODE_I]) {
		//	gMesh1.mTranslate.m_uAngle = +1.0f;
		//}
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
		if (state[SDL_SCANCODE_Q]) {
			app().mActor.movementSpeed = 0.01f;
		}
		if (state[SDL_SCANCODE_E]) {
			app().mActor.movementSpeed = 5.0f;
		}
		if (state[SDL_SCANCODE_ESCAPE]) {
			app().mQuit = true;
		}
	}
}