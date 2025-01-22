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
	LClick = false;
	mLastkey_Relevant = false;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			std::cout << "Goodbye!" << std::endl;
			app().mQuit = true;
		}
		else if (e.type == SDL_EVENT_MOUSE_MOTION) {
			mousePos.x = (e.motion.x / (float)app().mWindowWidth - (float)app().viewportOffset.x / (float)app().mWindowWidth) * 1920.0f;
			mousePos.y = (1.0f - e.motion.y / (float)app().mWindowHeight + (float)app().viewportOffset.y / (float)app().mWindowHeight) * 1080.0f;
			//680.680 
			//1000.760

			//std::cout << "Mouse motion: " << glm::to_string(mousePos) << (float)app().viewportOffset.x / (float)app().mWindowWidth << std::endl;
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				LClick = true;
			}
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			mLastkey_Relevant = true;
			mLastKey_Keycode = e.key.key;
			mLastKey_Scancode = e.key.scancode;
		}
		if (e.type == SDL_EVENT_KEY_UP) {



			if (e.key.scancode == mJUMP_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::SPACE)] = false;
				app().mMovementHandler.mSpacebarOneShot = true;
			} else if (e.key.scancode == mDUCK_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::DUCK)] = false;
				app().mMovementHandler.mDuckOneShot = true;
			} else if (e.key.scancode == mMOVE_LEFT_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = false;
			} else if (e.key.scancode == mMOVE_RIGHT_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = false;
			} else if (e.key.scancode == mMOVE_UP_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_UP)] = false;
			} else if (e.key.scancode == mMOVE_DOWN_Bind) {
				app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)] = false;
			}
			


		}
		const bool* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_T]) {
			app().LoadGame(true);
		}
		if (state[SDL_SCANCODE_E]) {
			//app().LoadGame();
		}
		if (state[mJUMP_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::SPACE)] = true;
		}
		if (state[mMOVE_UP_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_UP)] = true;
		}
		if (state[mMOVE_DOWN_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)] = true;
		}
		if (state[mDUCK_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::DUCK)] = true;
		}
		if (state[mMOVE_LEFT_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] = true;
		}
		if (state[mMOVE_RIGHT_Bind]) {
			app().mMovementHandler.mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] = true;
		}
		if (state[SDL_SCANCODE_P]) {
			//app().mSceneManager.mUIScenes.mTitleScreenActive = false;
			app().mPause = false;
		}
		if (state[SDL_SCANCODE_O]) {
			//app().mSceneManager.mUIScenes.mTitleScreenActive = true;
			app().mPause = true;
		}
		if (state[SDL_SCANCODE_GRAVE]) {
			app().mQuit = true;
		}
	}

	if (app().mSceneManager.mMainMenuActive == true) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}		
	}
	else if (app().mSceneManager.mUIScenes.mTitleScreenActive == true) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mInGameTitleButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}
	}
	else if (app().mPause) {
		for (const auto& pair : app().mSceneManager.mUIScenes.mInGamePauseButtonMap) {
			if (pair.second.mCreated) {
				if (PointVsRect((glm::vec2)mousePos, pair.second.mTriggerSize, pair.second.mTriggerPos)) {
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetHoverState(true);

					if (app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].ChangeColor(glm::vec3(1.0f));
					}


					if (LClick) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(true);
					}
					else {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(false);
					}
				}
				else {
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetHoverState(false);
					app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].SetPressState(false);
					if (app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].CheckHoverStateChange() && app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].mHoverReactive) {
						app().mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].ChangeColor(glm::vec3(0.7f));
					}
				}
			}
		}
	}




}