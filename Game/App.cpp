#include "App.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <algorithm>
#include "Sign.hpp"
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_mixer.h>
#include <glm/gtx/norm.hpp>
#include <random>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/exterior_product.hpp>

App::App() : mAnimationHandler(mTextureHandler) {
	StartUp();
}

App::~App() {
	ShutDown();
}

void App::StartUp() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "SDL3 could not initialize video subsystem or audio subsystem" << std::endl;
		exit(1);
	}

	SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
	SDL_Rect bounds;

	if (SDL_GetDisplayBounds(displayID, &bounds) == 0) {
		SDL_Log("Failed to get display bounds: %s", SDL_GetError());
	}

	if (bounds.h < 1080) {
		if (bounds.h < 720) {
			mWindowHeight = 480;
			mWindowWidth = 854;
			currentResolutionIndex = 0;
		}
		else {
			mWindowHeight = 720;
			mWindowWidth = 1280;
			currentResolutionIndex = 1;
		}
	}

	screenSize.x = bounds.w;
	screenSize.y = bounds.h;

	resolutions.push_back(glm::ivec2(854, 480));
	resolutions.push_back(glm::ivec2(1280, 720));
	resolutions.push_back(glm::ivec2(1366, 768));
	resolutions.push_back(glm::ivec2(1600, 900));
	resolutions.push_back(glm::ivec2(1920, 1080));
	resolutions.push_back(glm::ivec2(2560, 1440));
	resolutions.push_back(glm::ivec2(3840, 2160));
	resolutions.push_back(glm::ivec2(5120, 2880));
	resolutions.push_back(glm::ivec2(7680, 4320));

	windowModes.push_back("windowed");
	windowModes.push_back("fullscreen");

	if (IMG_Init(IMG_INIT_PNG) == 0) {
		std::cerr << "SDL3_image could not be initialized" << std::endl;
		exit(1);
	}
	if (Mix_OpenAudio(0, NULL) < 0)
	{
		std::cerr << "SDL3_mixer could not be initialized" << std::endl;
		exit(1);
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	if (debugMode) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mWindow = SDL_CreateWindow("Never Gonna Give You Up", mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL);

	SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	if (mWindow == nullptr) {
		std::cout << "SDL_Window was not able to be created" << std::endl;
		exit(1);
	}

	mGlContext = SDL_GL_CreateContext(mWindow);
	if (mGlContext == nullptr) {
		std::cout << "OpenGL context not available" << std::endl;
		exit(1);
	}
	if (vsync == false) {
		SDL_GL_SetSwapInterval(0);
	}
	else {
		SDL_GL_SetSwapInterval(1);
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Glad was not initialized" << std::endl;
		exit(1);
	}
	if (debugMode) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void App::PostStartUp() {
	mCamera.SetProjectionMatrix(1920.0f);

	mGeneralShaderProgram.CreateShaderProgram("shaders/vertexGeneral.glsl", "shaders/fragmentGeneral.glsl");
	mTextShaderProgram.CreateShaderProgram("shaders/vertexText.glsl", "shaders/fragmentText.glsl");
	mBackgroundShaderProgram.CreateShaderProgram("shaders/vertexBackground.glsl", "shaders/fragmentBackground.glsl");
	mBackgroundFramebufferShaderProgram.CreateShaderProgram("shaders/vertexBackgroundFramebuffer.glsl", "shaders/fragmentBackgroundFramebuffer.glsl");

	mTextRenderer.Init((const char*)"assets/Fonts/PixelOperator8-Bold.ttf", 96);

	mPipelineProgram.CreatePipelineProgram();
	mPipelineProgram.BindPipelineProgram();
	mPipelineProgram.BindShaderProgram(mGeneralShaderProgram.ID);

	mBatchRenderer.StartUp(&mGeneralShaderProgram, mPipelineProgram.ID);

	//SDL_Surface* tileset = mTextureHandler.LoadSurface("assets/Level/tiles128up.png");
	
	mTextureHandler.InitTextureArray(GL_RGBA8, 512, 512, 2000);

	uint32_t whiteTextureData[128 * 128];
	for (int i = 0; i < 128 * 128; i++) {
		whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	}
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 128, 128, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.mLayersUsed[0]++;

	//std::vector<SDL_Surface*> tiles = mTextureHandler.CutTileset(tileset, 128, 128);
	//for (int i = 0; i < tiles.size(); i++) {
	//	mTextureHandler.LoadTexture(tiles[i], GL_RGBA, mTextureHandler.mLayersUsed[0], 0);
	//}

	mAnimationHandler.Init(512);

	mTextOut.Init(mTextureHandler, "assets/Level/Text.png");

	Mix_AllocateChannels(20);
	mAudioHandler.LoadSounds();
	mEscapePortal.mSprite.mVertexData.Size = mAnimationHandler.EscapePortalAnimation.Size * mEscapePortal.mSizeMultiplier;

	int maxCombinedUnits, maxFragmentUnits, maxVertexUnits, maxArrayLayers;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedUnits);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentUnits);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);

	printf("Max Combined Texture Units: %d\n", maxCombinedUnits);
	printf("Max Fragment Texture Units: %d\n", maxFragmentUnits);
	printf("Max Vertex Texture Units: %d\n", maxVertexUnits);
	printf("Max Array Texture Layers: %d\n", maxArrayLayers);

	mTextureHandler.LoadAll(0);

	mSceneManager.mUIScenes.LoadMainMenuBackground("assets/UI/Data/Layout/40p.json", mTextureHandler.mUI_BordersT_Offset);

	mBackgroundRenderer.Init();

	mBackgroundRenderer.LoadMenuBackground("assets/UI/background4k.png", &mBackgroundShaderProgram, mPipelineProgram.ID, &mTextureHandler);

	mBackgroundRenderer.LoadLevelBackground(&mBackgroundShaderProgram, &mBackgroundFramebufferShaderProgram, mPipelineProgram.ID, &mTextureHandler);

	mAudioHandler.SetGlobalSFXVolume(SFXVolume);
	mAudioHandler.SetGlobalMusicVolume(MusicVolume);
}

void App::LoadGame(const bool retry) {
	mBlackHole.Reset();
	mStateMachine.Reset();
	mActor.Reset(mAnimationHandler.FallAnimation.Size, glm::vec2(400.0f, 350.0f));
	//mActor.Reset(mAnimationHandler.FallAnimation.Size, glm::vec2(17700.0f, 350.0f));
	mSceneManager.mUIScenes.mTitleScreenActive = false;
	// reset movement handler
	mMovementHandler.mLookDirection = LookDirections::RIGHT;
	titleScreenDarkened = false;
	newBestTimeMessageOneShot = false;

	// reset camera
	mCamera.mCameraOffset = glm::vec2(0.0f, 0.0f);

	// reset app
	gameStarted = false;
	titleScreenAlpha            = 0.0f;
	titleScreenMusicVolume      = 128;
	titleScreenAlphaTimer       = 0.0f;
	titleScreenMessageTimer     = 0.0f;
	startMessageTimer           = 0.0f;
	titleScreenMusicVolumeTimer = 0.0f;
	mAudioHandler.mInitialMusicVolume = 128;
	mAudioHandler.SetGlobalMusicVolume(MusicVolume);

	// reset and load level
	//mLevel.mBlocks.clear();
	//mLevel.LoadLevelJson("levels/GameLevels/32p/Test_Level.json");
	//mLevel.BuildLevel();
	//for (int i = 0; i < mLevel.mBlocks.size(); i++) {
	//	mSceneManager.mCurrentBlocks->at(i).Update();
	//}
	// 
	//mSceneManager.mCurrentBlocks = &mSceneManager.mLevelScene.mLevelBlocks;
	//mSceneManager.mLevelActive = true;
	//mSceneManager.mMainMenuActive = false;

	//mSceneManager.mUIScenes.LoadMainMenuBackground("assets/UI/Data/Layout/test40.json", mTextureHandler.mUI_BordersT_Offset);

	Mix_HaltMusic();
	Mix_HaltChannel(0);
	Mix_HaltChannel(1);
	Mix_HaltChannel(2);
	Mix_HaltChannel(3);
	Mix_HaltChannel(4);
	Mix_HaltChannel(5);
	Mix_HaltChannel(6);
	Mix_HaltChannel(7);
	Mix_HaltChannel(8);
	Mix_HaltChannel(9);
	Mix_HaltChannel(10);
	Mix_HaltChannel(11);
	Mix_HaltChannel(12);
	Mix_HaltChannel(13);
	Mix_HaltChannel(14);

	Mix_HaltChannel(17);
	Mix_HaltChannel(18);
	Mix_HaltChannel(19);

	if (!retry) {
		if (mSceneManager.mUIScenes.mActiveTab == MenuTabs::LEVELS) {
			mSceneManager.mUIScenes.TranslateMenuInstant(glm::vec2(-1920.0f, 0.0f));
		}

		mSceneManager.mUIScenes.LoadMainMenuSettingsTab(mTextureHandler.mUI_BordersT_Offset, mTextureHandler.mUI_ArrowsT_Offset);
		mSceneManager.mUIScenes.LoadMainMenuLevelsTab(mTextureHandler.mUI_BordersT_Offset);
		mSceneManager.mUIScenes.LoadTitleScreen(mTextureHandler.mUI_BordersT_Offset);
		mSceneManager.mUIScenes.LoadMainMenuControlsTab(mTextureHandler.mUI_BordersT_Offset);
		mSceneManager.LoadMainMenu(mTextureHandler.mUI_BordersT_Offset);
	}
	else if (retry) {
		mSceneManager.ReloadCurrentLevel(mTextureHandler.mBaseT_Offset, mAudioHandler.IntroMusic);
	}

	mSceneManager.mUIScenes.LoadPauseMenu(mTextureHandler.mUI_BordersT_Offset);


	pauseTime = 0;
	levelTime = 0;
	levelTimeTotal = 0;

	levelTime_seconds = 0;
	levelTime_minutes = 0;
	levelTime_centiseconds = 0;

	//mSceneManager.mUIScenes.mTestButton.CreateBoxButtonCentered(glm::ivec2(23, 3), glm::vec2(960.0f, 540.0f), 30, mTextureHandler.mUI_BordersT_Offset);
	//
	//
	//mSceneManager.mCurrentBlocks = &mSceneManager.mUIScenes.mBackgroundBlocks;
	//mSceneManager.mLevelActive = false;
	//mSceneManager.mMainMenuActive = true;

	//mSceneManager.LoadGameLevel("levels/GameLevels/32p/Level_1.json", &mLevel, mTextureHandler.mBaseT_Offset);

	//mSceneManager.LoadMenuLayout("assets/UI/Data/Layout/test40.json");
	//mSceneManager.LoadMainMenu(mTextureHandler.mUI_BordersT_Offset);

	// reset music
	//				mSceneManager.mCurrentBlocks = &mSceneManager.mLevelScene.mLevelBlocks;
	//			mSceneManager.mLevelActive = true;
	//			mSceneManager.mMainMenuActive = false;

}


void App::MainLoop() {
	SDL_WarpMouseInWindow(mWindow, mWindowWidth / 2, mWindowHeight / 2);
	TimePoint1 = std::chrono::system_clock::now();
	TimePoint2 = std::chrono::system_clock::now();
	while (!mQuit) {
		mInputManager.Input();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		Update();

		SDL_GL_SwapWindow(mWindow);
	}
}

void App::UpdatePlayground() {

	if (mSceneManager.mMainMenuActive && mSceneManager.mUIScenes.mNextTabLoaded) {
		switch (mSceneManager.mUIScenes.mActiveTab) {
		case MenuTabs::MAIN:

			if (mSceneManager.mUIScenes.mButtonMap["PLAY"].GetPressState()) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::LEVELS;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::LEVELS;
			}

			if (mSceneManager.mUIScenes.mButtonMap["QUIT"].GetPressState()) {
				mQuit = true;
			}

			if (mSceneManager.mUIScenes.mButtonMap["SETTINGS"].GetPressState()) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::SETTINGS;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::SETTINGS;
			}
			break;
		case MenuTabs::SETTINGS:
			if (mSceneManager.mUIScenes.mButtonMap["BACK"].GetPressState() || ((mInputManager.mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && mInputManager.mLastkey_Relevant)) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::MAIN;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::MAIN;
			}

			if (mSceneManager.mUIScenes.mButtonMap["WINDOW_MODE"].GetPressState()) {
				if (currentWindowModeIndex + 1 <= windowModes.size() - 1) {
					currentWindowModeIndex += 1;
				}
				else {
					currentWindowModeIndex = 0;
				}
				if (windowModes[currentWindowModeIndex] == "windowed") {
					SDL_SetWindowSize(mWindow, resolutions[currentResolutionIndex].x, resolutions[currentResolutionIndex].y);
					mWindowWidth = resolutions[currentResolutionIndex].x;
					mWindowHeight = resolutions[currentResolutionIndex].y;

					mBackgroundRenderer.ResizeFramebuffer(resolutions[currentResolutionIndex]);

					viewportOffset.x = 0;
					viewportOffset.y = 0;

					glViewport(0, 0, mWindowWidth, mWindowHeight);
					SDL_SetWindowFullscreen(mWindow, false);
				}
				else if (windowModes[currentWindowModeIndex] == "fullscreen") {
					SDL_SetWindowFullscreen(mWindow, true);

					int Width;
					int Height;

					SDL_GetWindowSize(mWindow, &Width, &Height);

					glm::ivec2 resTemp = glm::ivec2(Width, Height);

					int i = 0;
					for (; resolutions[i].y != screenSize.y; i++) {

					}

					mBackgroundRenderer.ResizeFramebuffer(resTemp);

					SDL_SetWindowSize(mWindow, resTemp.x, resTemp.y);
					mWindowWidth = resTemp.x;
					mWindowHeight = resTemp.y;

					viewportOffset.x = (screenSize.x - mWindowWidth) / 2;
					viewportOffset.y = (screenSize.y - mWindowHeight) / 2;

					glViewport(viewportOffset.x, viewportOffset.y, mWindowWidth, mWindowHeight);

				}
			}

			if (windowModes[currentWindowModeIndex] != "fullscreen") {
				if (mSceneManager.mUIScenes.mButtonMap["RESOLUTION_DOWN"].GetPressState()) {
					if (currentResolutionIndex - 1 >= 0) {
						currentResolutionIndex -= 1;

						mBackgroundRenderer.ResizeFramebuffer(resolutions[currentResolutionIndex]);

						SDL_SetWindowSize(mWindow, resolutions[currentResolutionIndex].x, resolutions[currentResolutionIndex].y);
						mWindowWidth = resolutions[currentResolutionIndex].x;
						mWindowHeight = resolutions[currentResolutionIndex].y;

						glViewport(0, 0, mWindowWidth, mWindowHeight);
						std::cout << glm::to_string(resolutions[currentResolutionIndex]) << std::endl;
					}
				}

				if (mSceneManager.mUIScenes.mButtonMap["RESOLUTION_UP"].GetPressState()) {
					if (currentResolutionIndex + 1 <= resolutions.size() - 1 && resolutions[currentResolutionIndex + 1].y <= screenSize.y) {
						currentResolutionIndex += 1; 

						mBackgroundRenderer.ResizeFramebuffer(resolutions[currentResolutionIndex]);

						SDL_SetWindowSize(mWindow, resolutions[currentResolutionIndex].x, resolutions[currentResolutionIndex].y);
						mWindowWidth = resolutions[currentResolutionIndex].x;
						mWindowHeight = resolutions[currentResolutionIndex].y;
						glViewport(0, 0, mWindowWidth, mWindowHeight);
						std::cout << glm::to_string(resolutions[currentResolutionIndex]) << std::endl;
					}
				}
			}
			if (mSceneManager.mUIScenes.mButtonMap["AUTO_RETRY"].GetPressState()) {
				if (autoRestart) {
					autoRestart = false;
				}
				else if (!autoRestart) {
					autoRestart = true;
				}
			}
			if (mSceneManager.mUIScenes.mButtonMap["SFX_VOLUME_DOWN"].GetPressState()) {
				if (SFXVolume > 0) {
					SFXVolume -= 1;
				}
				mAudioHandler.SetGlobalSFXVolume(SFXVolume);
			}
			if (mSceneManager.mUIScenes.mButtonMap["SFX_VOLUME_UP"].GetPressState()) {
				if (SFXVolume < 10) {
					SFXVolume += 1;
				}
				mAudioHandler.SetGlobalSFXVolume(SFXVolume);
			}
			if (mSceneManager.mUIScenes.mButtonMap["MUSIC_VOLUME_DOWN"].GetPressState()) {
				if (MusicVolume > 0) {
					MusicVolume -= 1;
				}
				mAudioHandler.SetGlobalMusicVolume(MusicVolume);
			}
			if (mSceneManager.mUIScenes.mButtonMap["MUSIC_VOLUME_UP"].GetPressState()) {
				if (MusicVolume < 10) {
					MusicVolume += 1;
				}
				mAudioHandler.SetGlobalMusicVolume(MusicVolume);
			}
			if (mSceneManager.mUIScenes.mButtonMap["CONTROLS"].GetPressState()) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::CONTROLS;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::CONTROLS;
			}

			//std::cout << SFXVolume << ", " << MusicVolume << std::endl;

			//if (autoRestart) {
			//	mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "On", 960.0f + 1920.0f, 405.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["AUTO_RETRY"].mModelMatrix);
			//}
			//else if (!autoRestart) {
			//	mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "Off", 960.0f + 1920.0f, 405.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["AUTO_RETRY"].mModelMatrix);
			//}
			//
			//mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, std::to_string(SFXVolume), 960.0f + 1920.0f, 535.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["SFX_VOLUME"].mModelMatrix);
			//mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, std::to_string(MusicVolume), 960.0f + 1920.0f, 665.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["MUSIC_VOLUME"].mModelMatrix);
			//
			//mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, std::format("{}x{}", mWindowWidth, mWindowHeight), 960.0f + 1920.0f, 925.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["RESOLUTION"].mModelMatrix);
			//mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, windowModes[currentWindowModeIndex], 960.0f + 1920.0f, 795.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["WINDOW_MODE"].mModelMatrix);
			break;
		case MenuTabs::LEVELS:
			if (mSceneManager.mUIScenes.mButtonMap["BACK_LEVEL_TAB"].GetPressState() || ((mInputManager.mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && mInputManager.mLastkey_Relevant)) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::MAIN;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::RIGHT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::MAIN;
			}
			if (mSceneManager.mUIScenes.mButtonMap["LEVEL_1"].GetPressState()) {
				mSceneManager.LoadLevel(Levels::LEVEL_1, mTextureHandler.mBaseT_Offset, mAudioHandler.IntroMusic);
			}
			if (mSceneManager.mUIScenes.mButtonMap["TEST_LEVEL"].GetPressState()) {
				mSceneManager.LoadLevel(Levels::TEST_LEVEL, mTextureHandler.mBaseT_Offset, mAudioHandler.IntroMusic);
			}
			//if (best_levelTime_minutes == 0 && best_levelTime_seconds == 0 && best_levelTime_centiseconds == 0) {
			//	mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "Best Time: Not played", 1300.0f - 1920.0f, 960.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["LEVEL_1"].mModelMatrix, false, true, true);
			//}
			//else {
			//	mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "Best Time: " + std::format("{}:{:02}:{:02}", best_levelTime_minutes, best_levelTime_seconds, best_levelTime_centiseconds), 1300.0f - 1920.0f, 960.0f, 0.25f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mSceneManager.mUIScenes.mButtonMap["LEVEL_1"].mModelMatrix, false, true, true);
			//}

			break;
		case MenuTabs::CONTROLS:
			if (mSceneManager.mUIScenes.mButtonMap["BACK_CONTROLS_TAB"].GetPressState() || ((mInputManager.mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && mInputManager.mLastkey_Relevant)) {
				mSceneManager.mUIScenes.mNextTabLoaded = false;
				mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mNextTab = MenuTabs::SETTINGS;
				mSceneManager.mUIScenes.mMenuMoveDirection = MenuMoveDirection::LEFT;
				mSceneManager.mUIScenes.mActiveTab = MenuTabs::SETTINGS;
			}
			if (mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].GetPressState()) {
				mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = true;
				mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].GetPressState()) {
				mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = true;
				mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].GetPressState()) {
				mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = true;
				mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
			}
			if (mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].GetPressState()) {
				mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = true;
			}
			break;
		}
		if ((mInputManager.mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && mInputManager.mLastkey_Relevant) {
			Mix_PlayChannel(18, mAudioHandler.EscapeClick, 0);
		}
	}

	if (!mSceneManager.mUIScenes.mNextTabLoaded) {
		switch (mSceneManager.mUIScenes.mMenuMoveDirection) {
		case MenuMoveDirection::LEFT:
			mSceneManager.mUIScenes.TranslateMenuSmooth(glm::vec2(1920.0f, 0.0f), deltaTime, 3000.0f);
			break;
		case MenuMoveDirection::RIGHT:
			mSceneManager.mUIScenes.TranslateMenuSmooth(glm::vec2(-1920.0f, 0.0f), deltaTime, 3000.0f);
			break;
		}
	}

	if (mSceneManager.mMainMenuActive) {
		switch (mSceneManager.mUIScenes.mActiveTab) {
		case MenuTabs::MAIN:
			break;
		case MenuTabs::SETTINGS:
			mSceneManager.mUIScenes.mDynamicTextMap["tRESOLUTION"].Text = std::format("{}x{}", mWindowWidth, mWindowHeight);

			mSceneManager.mUIScenes.mDynamicTextMap["tWINDOW_MODE"].Text = windowModes[currentWindowModeIndex];
			
			mSceneManager.mUIScenes.mDynamicTextMap["tMUSIC_VOLUME"].Text = std::to_string(MusicVolume);
			
			mSceneManager.mUIScenes.mDynamicTextMap["tSFX_VOLUME"].Text = std::to_string(SFXVolume);

			if (autoRestart) {
				mSceneManager.mUIScenes.mDynamicTextMap["tAUTO_RETRY"].Text = "On";
			}
			else if (!autoRestart) {
				mSceneManager.mUIScenes.mDynamicTextMap["tAUTO_RETRY"].Text = "Off";
			}
			break;
		case MenuTabs::LEVELS:
			if (best_levelTime_minutes == 0 && best_levelTime_seconds == 0 && best_levelTime_centiseconds == 0) {
				mSceneManager.mUIScenes.mDynamicTextMap["tLEVEL_1"].Text = "Best Time: Not played";
			}
			else {
				mSceneManager.mUIScenes.mDynamicTextMap["tLEVEL_1"].Text = "Best Time: " + std::format("{}:{:02}:{:02}", best_levelTime_minutes, best_levelTime_seconds, best_levelTime_centiseconds);
			}
			break;
		case MenuTabs::CONTROLS:
			if (mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting) {
				if (mInputManager.mLastkey_Relevant) {
					mInputManager.mMOVE_LEFT_Bind = mInputManager.mLastKey_Scancode;
					mSceneManager.mUIScenes.mButtonMap["LEFT_BIND"].mInteracting = false;
				}
				mSceneManager.mUIScenes.mDynamicTextMap["tLEFT_BIND"].Text = "_";
				mSceneManager.mUIScenes.mDynamicTextMap["tLEFT_BIND"].Possition.y = 950.0f;
			}
			else {
				mSceneManager.mUIScenes.mDynamicTextMap["tLEFT_BIND"].Text = SDL_GetKeyName(SDL_GetKeyFromScancode(mInputManager.mMOVE_LEFT_Bind, SDL_KMOD_NONE, false));
				mSceneManager.mUIScenes.mDynamicTextMap["tLEFT_BIND"].Possition.y = 960.0f;
			}
			if (mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting) {
				if (mInputManager.mLastkey_Relevant) {
					mInputManager.mMOVE_RIGHT_Bind = mInputManager.mLastKey_Scancode;
					mSceneManager.mUIScenes.mButtonMap["RIGHT_BIND"].mInteracting = false;
				}
				mSceneManager.mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Text = "_";
				mSceneManager.mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Possition.y = 850.0f;
			}
			else {
				mSceneManager.mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Text = SDL_GetKeyName(SDL_GetKeyFromScancode(mInputManager.mMOVE_RIGHT_Bind, SDL_KMOD_NONE, false));
				mSceneManager.mUIScenes.mDynamicTextMap["tRIGHT_BIND"].Possition.y = 860.0f;
			}
			if (mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting) {
				if (mInputManager.mLastkey_Relevant) {
					mInputManager.mJUMP_Bind = mInputManager.mLastKey_Scancode;
					mSceneManager.mUIScenes.mButtonMap["JUMP_BIND"].mInteracting = false;
				}
				mSceneManager.mUIScenes.mDynamicTextMap["tJUMP_BIND"].Text = "_";
				mSceneManager.mUIScenes.mDynamicTextMap["tJUMP_BIND"].Possition.y = 750.0f;
			}
			else {
				mSceneManager.mUIScenes.mDynamicTextMap["tJUMP_BIND"].Text = SDL_GetKeyName(SDL_GetKeyFromScancode(mInputManager.mJUMP_Bind, SDL_KMOD_NONE, false));
				mSceneManager.mUIScenes.mDynamicTextMap["tJUMP_BIND"].Possition.y = 760.0f;
			}
			if (mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting) {
				if (mInputManager.mLastkey_Relevant) {
					mInputManager.mDUCK_Bind = mInputManager.mLastKey_Scancode;
					mSceneManager.mUIScenes.mButtonMap["DUCK_BIND"].mInteracting = false;
				}
				mSceneManager.mUIScenes.mDynamicTextMap["tDUCK_BIND"].Text = "_";
				mSceneManager.mUIScenes.mDynamicTextMap["tDUCK_BIND"].Possition.y = 650.0f;
			}
			else {
				mSceneManager.mUIScenes.mDynamicTextMap["tDUCK_BIND"].Text = SDL_GetKeyName(SDL_GetKeyFromScancode(mInputManager.mDUCK_Bind, SDL_KMOD_NONE, false));
				mSceneManager.mUIScenes.mDynamicTextMap["tDUCK_BIND"].Possition.y = 660.0f;
			}
			break;
		}
	}





}

void App::Update() {

	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.
	// IT IS A MESS, I KNOW IT, I WILL REFACTOR IT.

	static int fps1;
	// delta time logic vvv
	TimePoint2 = std::chrono::system_clock::now();
	static auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsedTime = TimePoint2 - TimePoint1;
	static int frameCount = 0;
	frameCount++;
	// Our time per frame coefficient
	deltaTime = elapsedTime.count();
	if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1) {
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		auto fps = frameCount * 1000.0 / elapsedTime;

		fps1 = fps;

		//std::cout << "FPS: " << fps << std::endl;

		frameCount = 0;
		lastTime = currentTime;
	}
	static int o = 0;
	if (o < 20) {
		deltaTimeBuffer += deltaTimeRaw;
	}
	if (o == 20) {
		deltaTimeBuffer = 0;
		o = 0;
	}
	else {
		o++;
	}

	//std::cout << "Total: " << levelTimeTotal << ", Level: " << levelTime << ", Pause: " << pauseTime << std::endl;

	TimePoint1 = TimePoint2;
	// delta time logic ^^^
	if (mSceneManager.mMainMenuActive == true) {
		mBackgroundRenderer.RenderMenuBackground(&mBackgroundShaderProgram, mPipelineProgram.ID, mCamera.mUIModelMatrix, mCamera.GetProjectionMatrix());
	}
	else if (mSceneManager.mLevelActive == true) {
		if (!mPause) {
			mBackgroundRenderer.Update(mCamera.mCameraPosition, mActor.mVelocity, deltaTime);
		}
		mBackgroundRenderer.RenderLevelBackground(&mBackgroundShaderProgram, &mBackgroundFramebufferShaderProgram, mPipelineProgram.ID, mCamera.GetProjectionMatrix(), mBlackHole.AABBSize.x, mBlackHole.epicenterAABBPos +	mBlackHole.epicenterAABBSize / 2.0f, mCamera.mCameraPosition.x, gameStarted, deltaTime);
	}

	//std::cout << glm::to_string(mCamera.mCameraPosition) << std::endl;

	if (mSceneManager.mLevelActive) {
		if ((mInputManager.mLastKey_Scancode == SDL_SCANCODE_ESCAPE) && mInputManager.mLastkey_Relevant) {
			if (mPause) {
				mPause = false;
				pauseTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count();
				Mix_ResumeMusic();
				Mix_Resume(0);
				Mix_Resume(1);
				Mix_Resume(2);
				Mix_Resume(3);
				Mix_Resume(4);
				Mix_Resume(5);
				Mix_Resume(6);
				Mix_Resume(7);
				Mix_Resume(8);
				Mix_Resume(9);
				Mix_Resume(10);
				Mix_Resume(11);
				Mix_Resume(12);
				Mix_Resume(13);
				Mix_Resume(14);

				Mix_Resume(17);
				Mix_Resume(18);
				Mix_Resume(19);
			}
			else if (!mPause) {
				pauseTimer = std::chrono::high_resolution_clock::now();
				mPause = true;
			}
		}
	}

	if (mActor.mDead || mActor.mEscaped) {
		if (mAudioHandler.mMusicFadeoutTimer > mAudioHandler.mMusicFadeoutTime && mAudioHandler.mInitialMusicVolume > 0) {
			mAudioHandler.mInitialMusicVolume -= 1;
			mAudioHandler.mMusicFadeoutTimer = 0.0f;
		}
		if (mAudioHandler.mInitialMusicVolume < 0) {
			mAudioHandler.mInitialMusicVolume = 0;
		}
		mAudioHandler.mMusicFadeoutTimer += deltaTime;
		mAudioHandler.SetGlobalMusicVolume(MusicVolume);
	}



	// game start logic
	if (gameStarted == false && mActor.mVelocity.x != 0.0f) {
		Mix_HaltMusic();
		gameStarted = true;
		levelTimerTotal = std::chrono::high_resolution_clock::now();
	}

	if (gameStarted) {
		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(mAudioHandler.LoopMusic, 0);
		}
	}
	else {
		if (mSceneManager.mLevelActive) {
			startMessageTimer += deltaTime;
			if (startMessageTimer > startMessageTime) {
				startMessageTimer = 0.0f;
			}
			if (startMessageTimer >= 1.0f) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "Move to start", mActor.mPosition.x + 100.0f, mActor.mPosition.y + 200.0f, 0.5f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mActor.mModelMatrix);
			}
		}
	}

	if (mSceneManager.mMainMenuActive) {
		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(mAudioHandler.LoopMenuMusic, 0);
		}
	}

	// movement update
	// actor update
	if (!mPause) {
		mMovementHandler.Update(deltaTime, mActor, mSceneManager.mLevelActive);
		mActor.Update();
		if (!mMovementHandler.mLast_mCanDoubleJump && mMovementHandler.mCanDoubleJump && !mActor.mDead) {
			Mix_PlayChannel(19, mAudioHandler.DoubleJumpRecharge, 0);
		}
	}


	// black hole update
	if (gameStarted && !mPause) {
		mBlackHole.Update(mSceneManager.mCurrentBlocks, mActor, deltaTime, mAnimationHandler.BlackHoleBirthAnimation, mAnimationHandler.BlackHoleLoopAnimation, mAudioHandler.BlackHoleBorn, mAudioHandler.ConsumedByVoid, mAudioHandler.BlackHoleIdle, mAudioHandler.mGlobalSFXVolumeModifier);
	}

	//mBatchRenderer.DrawSeperatly(glm::vec2(0.0f, 0.0f), mBlackHole.AABBSize, glm::vec4(0.0f, 0.0f, 1.0f, 0.2f), mCamera.GetProjectionMatrix());

	// dead logic
	if (mActor.mPosition.y < -500.0f && !mActor.mDead) {
		if (!mActor.mDead) {
			mActor.mVelocity = glm::vec2(0.0f, 0.0f);
			mActor.mFellDown = true;
			mActor.mDead = true;
		}
	}
	
	if (mActor.mDead && !mActor.mConsumedByBlackHole) {
		if (mActor.mDeadSoundOneShot) {
			Mix_PlayChannel(14, mAudioHandler.FellDown, 0);
			mActor.mDeadSoundOneShot = false;
		}
	}

	// collision update
	// camera update
	if (!mPause) {
		CollisionUpdate(mSceneManager.mCurrentBlocks, mActor, mMovementHandler.mLeftWallHug, mMovementHandler.mRightWallHug, deltaTime, mMovementHandler.mIsGrounded, mMovementHandler.mIsWallMountableL, mMovementHandler.mIsWallMountableR);
		mCamera.Update(mActor.mVelocity, mActor.mScreenPosition, deltaTime);
	}

	for (auto& pair : mSceneManager.mUIScenes.mButtonMap) {
		mSceneManager.mUIScenes.mButtonMap[pair.first].Update(mCamera.mUIModelMatrix, mAudioHandler.ButtonHover, mAudioHandler.ButtonClick);
	}

	for (auto& pair : mSceneManager.mUIScenes.mInGameTitleButtonMap) {
		mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].Update(mCamera.mUIModelMatrix, mAudioHandler.ButtonHover, mAudioHandler.ButtonClick);
	}
	for (auto& pair : mSceneManager.mUIScenes.mInGamePauseButtonMap) {
		mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].Update(mCamera.mUIModelMatrix, mAudioHandler.ButtonHover, mAudioHandler.ButtonClick);
	}

	mSceneManager.mUIScenes.Update(mCamera.mUIModelMatrix);

	// screen position calculation
	mActor.mScreenPosition = mCamera.GetProjectionMatrix() * glm::vec4(mActor.mPosition.x + mActor.mSprite.mVertexData.Size.x / 2, mActor.mPosition.y + mActor.mSprite.mVertexData.Size.y / 2, 0.0f, 1.0f);

	//mBatchRenderer.DrawSeperatly(glm::vec2(0.0f, 0.0f), glm::vec2(5.0f, 1080.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), mCamera.GetProjectionMatrix());

	// level render vvv
	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());

	//std::cout << mSceneManager.mCurrentBlocks->size() << std::endl;

	if (mSceneManager.mLevelActive == true) {
		if (gameStarted) {
			if (mBlackHole.mSprite.mVertexData.Position.x + mBlackHole.mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mBlackHole.mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
				mBatchRenderer.DrawInBatch(mBlackHole.mSprite.mVertexData.Position, mBlackHole.mSprite.mVertexData.Size, mBlackHole.mSprite.mVertexData.TextureIndex, mAnimationHandler.BlackHoleLoopAnimation.Size, mBlackHole.mSprite.mVertexData.TexturePosition);
			}
		}

		// buffer of block id that are being sucked into the black hole and visible
		std::vector<int> flyingBlocks;

		for (int i = 0; i < mSceneManager.mCurrentBlocks->size(); i++) {
			if (mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mSceneManager.mCurrentBlocks->at(i).mIsVisible == true && mSceneManager.mCurrentBlocks->at(i).mIsSucked == false) {
				mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.TextureIndex), glm::vec2(0.25f, 0.25f));
			}
			else if (mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mSceneManager.mCurrentBlocks->at(i).mIsVisible == true && mSceneManager.mCurrentBlocks->at(i).mIsSucked == true) {
				flyingBlocks.push_back(i);
			}
		}

		mEscapePortal.Update(mAnimationHandler.EscapePortalAnimation, deltaTime, mActor, mAudioHandler.PortalEscape, mAudioHandler.PortalIdle, mAudioHandler.mGlobalSFXVolumeModifier);
		if (mEscapePortal.mSprite.mVertexData.Position.x + mEscapePortal.mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
			&& mEscapePortal.mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
			mBatchRenderer.DrawInBatch(mEscapePortal.mSprite.mVertexData.Position, mEscapePortal.mSprite.mVertexData.Size, mEscapePortal.mSprite.mVertexData.TextureIndex, mAnimationHandler.EscapePortalAnimation.Size, mEscapePortal.mSprite.mVertexData.TexturePosition);
		}

		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());

		for (int i = 0; i < flyingBlocks.size(); i++) {
			if (mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
				//mBatchRenderer.DrawSeperatly(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Size, mCamera.GetProjectionMatrix(),
				//	static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.TextureIndex), glm::vec2(0.25f, 0.25f), glm::vec2(0.0f, 0.0f), mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mModelMatrix, false);
				mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.TextureIndex), glm::vec2(0.25f, 0.25f), glm::vec2(0.0f, 0.0f), mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mFlyAngle, mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mFlyAngleOrientation);



			}
		}

		mBatchRenderer.EndBatch();
		mBatchRenderer.FlushFly();
	} // level render ^^^ menu render vvv
	else if (mSceneManager.mMainMenuActive == true) {
		for (const auto& pair : mSceneManager.mUIScenes.mButtonMap) {
			if (RectVsRect(glm::vec2(-10.0f, -10.0f), glm::vec2(2000.0f, 1160.0f), pair.second.mTriggerPos, pair.second.mTriggerSize)) {
				// idk
			}
			mSceneManager.mUIScenes.mButtonMap[pair.first].Render(&mBatchRenderer, mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
		}
		if (mSceneManager.mMainMenuActive == true) {
			for (int i = 0; i < mSceneManager.mUIScenes.mTextToRender.size(); i++) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mTextToRender[i].Text, mSceneManager.mUIScenes.mTextToRender[i].Possition.x, mSceneManager.mUIScenes.mTextToRender[i].Possition.y, mSceneManager.mUIScenes.mTextToRender[i].Scale, mSceneManager.mUIScenes.mTextToRender[i].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mTextToRender[i].ModelMatrix, mSceneManager.mUIScenes.mTextToRender[i].CenteredX, mSceneManager.mUIScenes.mTextToRender[i].CenteredY, mSceneManager.mUIScenes.mTextToRender[i].RightSided);
			}
			for (auto& pair : mSceneManager.mUIScenes.mDynamicTextMap) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Text, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Possition.x, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Possition.y, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Scale, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mDynamicTextMap[pair.first].ModelMatrix, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].CenteredX, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].CenteredY, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].RightSided);
			}
		}
		
		UpdatePlayground();

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		for (int i = 0; i < mSceneManager.mCurrentBlocks->size(); i++) {
			mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.TextureIndex), glm::vec2(0.03125f, 0.03125f), glm::vec2(0.0f, 0.0f), false, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Color);
		}
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush(mCamera.mUIModelMatrix);
	}

	// menu render ^^^


	// state machine update
	if (mSceneManager.mLevelActive && !mPause) {
		mStateMachine.Update(mMovementHandler, mAnimationHandler, mAudioHandler, mActor, deltaTime);
	}

	glm::vec2 adadaad = glm::vec2(mActor.mSprite.mVertexData.Position.x + (mStateMachine.mCurrentActorDrawSize.x / 2.0f) - (mActor.mDoubleJumpOrb.Size.x / 2.0f), mActor.mSprite.mVertexData.Position.y + mStateMachine.mCurrentActorDrawSize.y + 40.0f);

	// actor render
	if (mActor.mIsVisible == true && mSceneManager.mLevelActive == true) {
		mBatchRenderer.DrawSeperatly(mActor.mSprite.mVertexData.Position, mStateMachine.mCurrentActorDrawSize, mCamera.GetProjectionMatrix(),
			mStateMachine.mCurrentActorTextureIndex, mStateMachine.mCurrentActorTextureSize, mStateMachine.mCurrentActorTexturePosition, mActor.mModelMatrix, mStateMachine.mActorFlipped);	
		if (!mActor.mDead) {
			mBatchRenderer.DrawSeperatly(adadaad, mActor.mDoubleJumpOrb.Size, mCamera.GetProjectionMatrix(),
				mTextureHandler.mOrbT_Offset + 1, glm::vec2(0.1875f, 0.1875f), mActor.mDoubleJumpOrb.TexturePosition, mActor.mModelMatrix, false);
			if (mMovementHandler.mCanDoubleJump) {
				mBatchRenderer.DrawSeperatly(adadaad, mActor.mDoubleJumpOrb.Size, mCamera.GetProjectionMatrix(),
					mTextureHandler.mOrbT_Offset, glm::vec2(0.1875f, 0.1875f), mActor.mDoubleJumpOrb.TexturePosition, mActor.mModelMatrix, false);
			}
		}
		//mBatchRenderer.Test();
	}



	//if (mSceneManager.mChangingScene) {
	//	mSceneManager.SceneChangeAnim(deltaTime);
	//	mBatchRenderer.DrawSeperatly(glm::vec2(0.0f, 0.0f), mSceneManager.mCurtainSize, glm::vec4((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 1.0f), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
	//}
	//mBatchRenderer.DrawSeperatly(glm::vec2((1.0f + mActor.mScreenPosition.x) * 960.0f - mActor.mSprite.mVertexData.Size.x / 2, (1.0f + mActor.mScreenPosition.y) * 540.0f), glm::vec2(5.0f, 5.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
	//
	//mBatchRenderer.DrawSeperatly(mBlackHole.epicenterAABBPos, mBlackHole.epicenterAABBSize, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), mCamera.GetProjectionMatrix());

	if (mActor.mDead || mActor.mEscaped) {
		mSceneManager.mUIScenes.mTitleScreenActive = true;
	}

	if (mSceneManager.mLevelActive == true && mSceneManager.mUIScenes.mTitleScreenActive == true) {

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		//mBatchRenderer.DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4((71.0f / 256.0f), (17.0f / 256.0f), (7.0f / 256.0f), titleScreenAlpha));
		mBatchRenderer.DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4(0.0f, 0.0f, 0.0f, titleScreenAlpha));
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush(mCamera.mUIModelMatrix);

		if (titleScreenAlphaTimer > titleScreenAlphaTime && titleScreenAlpha < 0.5f) {
			titleScreenAlpha += 0.008f;
			titleScreenAlphaTimer = 0.0f;
		}
		if (titleScreenAlpha >= 0.5f && !titleScreenDarkened) {
			titleScreenAlpha = 0.5f;
			titleScreenDarkened = true;
		}
		titleScreenAlphaTimer += deltaTime;

		if (titleScreenMusicVolumeTimer > titleScreenMusicVolumeTime && titleScreenMusicVolume > 0) {
			titleScreenMusicVolume -= 1;
			titleScreenMusicVolumeTimer = 0.0f;
		}
		if (titleScreenMusicVolume < 0) {
			titleScreenMusicVolume = 0;
		}
		titleScreenMusicVolumeTimer += deltaTime;

		if (titleScreenDarkened) {
			if (autoRestart && !mActor.mEscaped) {
				LoadGame(true);
			}

			for (const auto& pair : mSceneManager.mUIScenes.mInGameTitleButtonMap) {
				mSceneManager.mUIScenes.mInGameTitleButtonMap[pair.first].Render(&mBatchRenderer, mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
			}

			if (!mActor.mEscaped) {
				std::string deathMessageText;
				glm::vec4 deathMessageColor = glm::vec4(1.0f);
				
				switch (mStateMachine.mActorDeathCause) {
				case DeathCause::LAVA:
					deathMessageText = "Tried to swim in lava";
					deathMessageColor = glm::vec4(1.0f, 0.42578125f, 0.12109375f, 1.0f);
					break;
				case DeathCause::FELL_DOWN:
					deathMessageText = "Fell out of the world";
					deathMessageColor = glm::vec4(0.0f, 0.7265625f, 1.0f, 1.0f);
					break;
				case DeathCause::BLACK_HOLE:
					deathMessageText = "Consumed by the void";
					deathMessageColor = glm::vec4(0.0f, 0.7265625f, 1.0f, 1.0f);
					break;
				default:
					deathMessageText = "Skill issue, git gud lol";
					break;
				}

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Text = "Game Over";
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Color = glm::vec3(1.0f, 0.0f, 0.14f);

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].Text = deathMessageText;
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].Color = deathMessageColor;

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTIME"].Text = "";

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tBEST_TIME"].Text = "";

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].Text = "";

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tRETRY"].Text = "Retry";
			} 
			else {
				if (((levelTime_minutes * 6000 + levelTime_seconds * 100 + levelTime_centiseconds) < (best_levelTime_minutes * 6000 + best_levelTime_seconds * 100 + best_levelTime_centiseconds)) || (best_levelTime_minutes == 0 && best_levelTime_seconds == 0 && best_levelTime_centiseconds == 0)) {
					best_levelTime_minutes = levelTime_minutes;
					best_levelTime_seconds = levelTime_seconds;
					best_levelTime_centiseconds = levelTime_centiseconds;
					newBestTimeMessageOneShot = true;
				}

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Text = "Escaped";
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTITLE"].Color = glm::vec3(0.8125f, 0.0f, 0.875f);

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tDEATH_CAUSE"].Text = "";

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tTIME"].Text = "Time: " + std::format("{}:{:02}:{:02}", levelTime_minutes, levelTime_seconds, levelTime_centiseconds);

				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tBEST_TIME"].Text = "Best Time: " + std::format("{}:{:02}:{:02}", best_levelTime_minutes, best_levelTime_seconds, best_levelTime_centiseconds);
				
				if (newBestTimeMessageOneShot) {
					mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].Text = "New Best Time!";
				}
				else {
					mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tNEW_BEST_TIME"].Text = "";
				}
				
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tRETRY"].Text = "Restart";


			}
			if (mSceneManager.mUIScenes.mInGameTitleButtonMap["RETRY"].GetPressState()) {
				LoadGame(true);
			}
			if (mSceneManager.mUIScenes.mInGameTitleButtonMap["AUTO_RETRY"].GetPressState()) {
				if (autoRestart) {
					autoRestart = false;
				}
				else if (!autoRestart) {
					autoRestart = true;
				}
			}
			if (mSceneManager.mUIScenes.mInGameTitleButtonMap["BACK_TO_MENU"].GetPressState()) {
				LoadGame();
			}

			if (autoRestart) {
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tAUTO_RETRY"].Text = "On";
			}
			else if (!autoRestart) {
				mSceneManager.mUIScenes.mInGameTitleDynamicTextMap["tAUTO_RETRY"].Text = "Off";
			}

			for (int i = 0; i < mSceneManager.mUIScenes.mInGameTitleTextToRender.size(); i++) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].Text, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].Possition.x, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].Possition.y, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].Scale, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mInGameTitleTextToRender[i].ModelMatrix, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].CenteredX, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].CenteredY, mSceneManager.mUIScenes.mInGameTitleTextToRender[i].RightSided);
			}

			for (auto& pair : mSceneManager.mUIScenes.mInGameTitleDynamicTextMap) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].Text, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].Possition.x, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].Possition.y, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].Scale, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].ModelMatrix, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].CenteredX, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].CenteredY, mSceneManager.mUIScenes.mInGameTitleDynamicTextMap[pair.first].RightSided);
			}
		}
	}
	else if (mSceneManager.mLevelActive && mPause) {
		Mix_PauseMusic();
		Mix_Pause(0);
		Mix_Pause(1);
		Mix_Pause(2);
		Mix_Pause(3);
		Mix_Pause(4);
		Mix_Pause(5);
		Mix_Pause(6);
		Mix_Pause(7);
		Mix_Pause(8);
		Mix_Pause(9);
		Mix_Pause(10);
		Mix_Pause(11);
		Mix_Pause(12);
		Mix_Pause(13);
		Mix_Pause(14);

		Mix_Pause(17);
		Mix_Pause(18);
		Mix_Pause(19);

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		mBatchRenderer.DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush(mCamera.mUIModelMatrix);

		for (const auto& pair : mSceneManager.mUIScenes.mInGamePauseButtonMap) {
			mSceneManager.mUIScenes.mInGamePauseButtonMap[pair.first].Render(&mBatchRenderer, mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
		}
		if (mSceneManager.mUIScenes.mInGamePauseButtonMap["CONTINUE"].GetPressState()) {
			mSceneManager.mUIScenes.mInGamePauseButtonMap["CONTINUE"].SetPressState(false);
			mPause = false;
			pauseTime += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count();
			Mix_ResumeMusic();
			Mix_Resume(0);
			Mix_Resume(1);
			Mix_Resume(2);
			Mix_Resume(3);
			Mix_Resume(4);
			Mix_Resume(5);
			Mix_Resume(6);
			Mix_Resume(7);
			Mix_Resume(8);
			Mix_Resume(9);
			Mix_Resume(10);
			Mix_Resume(11);
			Mix_Resume(12);
			Mix_Resume(13);
			Mix_Resume(14);

			Mix_Resume(17);
			Mix_Resume(18);
			Mix_Resume(19);
		}
		if (mSceneManager.mUIScenes.mInGamePauseButtonMap["AUTO_RETRY"].GetPressState()) {
			if (autoRestart) {
				autoRestart = false;
			}
			else if (!autoRestart) {
				autoRestart = true;
			}
		}
		if (mSceneManager.mUIScenes.mInGamePauseButtonMap["BACK_TO_MENU"].GetPressState()) {
			mSceneManager.mUIScenes.mInGamePauseButtonMap["BACK_TO_MENU"].SetPressState(false);
			mPause = false;
			LoadGame();
		}
		if (autoRestart) {
			mSceneManager.mUIScenes.mInGamePauseDynamicTextMap["tAUTO_RETRY"].Text = "On";
		}
		else if (!autoRestart) {
			mSceneManager.mUIScenes.mInGamePauseDynamicTextMap["tAUTO_RETRY"].Text = "Off";
		}

		for (int i = 0; i < mSceneManager.mUIScenes.mInGamePauseTextToRender.size(); i++) {
			mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].Text, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].Possition.x, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].Possition.y, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].Scale, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mInGamePauseTextToRender[i].ModelMatrix, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].CenteredX, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].CenteredY, mSceneManager.mUIScenes.mInGamePauseTextToRender[i].RightSided);
		}
		for (auto& pair : mSceneManager.mUIScenes.mInGamePauseDynamicTextMap) {
			mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].Text, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].Possition.x, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].Possition.y, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].Scale, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].ModelMatrix, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].CenteredX, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].CenteredY, mSceneManager.mUIScenes.mInGamePauseDynamicTextMap[pair.first].RightSided);
		}
	}

	levelTimeTotal = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - levelTimerTotal).count();
	if (gameStarted && !mActor.mEscaped && !mSceneManager.mUIScenes.mTitleScreenActive) {
		if (mPause) {
			levelTime = levelTimeTotal - (pauseTime + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - pauseTimer).count());
		}
		else {
			levelTime = levelTimeTotal - pauseTime;
		}
		if (!mPause) {
			levelTime_seconds = (levelTime / 1000) % 60;

			levelTime_minutes = (levelTime / 1000) / 60;

			levelTime_centiseconds = (levelTime % 1000) / 10;
		}
		mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "FPS: " + std::to_string(fps1), 80.0f, 1000.0f, 0.20f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix, false, false, false);

		mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, std::format("{}:{:02}:{:02}", levelTime_minutes, levelTime_seconds, levelTime_centiseconds), 1840.0f, 1000.0f, 0.35f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix, false, false, true);
	}
}

void App::UIUpdate() {

	if (mActor.mDead || mActor.mEscaped) {
		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		mBatchRenderer.DrawInBatch(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), titleScreenAlpha));
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush(mCamera.mUIModelMatrix);
		if (titleScreenAlphaTimer > titleScreenAlphaTime && titleScreenAlpha < 1.0f) {
			titleScreenAlpha += 0.004f;
			titleScreenAlphaTimer = 0.0f;
		}
		if (titleScreenAlpha > 1.0f) {
			titleScreenAlpha = 1.0f;
		}
		titleScreenAlphaTimer += deltaTime;

		if (titleScreenMusicVolumeTimer > titleScreenMusicVolumeTime && titleScreenMusicVolume > 0) {
			titleScreenMusicVolume -= 1;
			titleScreenMusicVolumeTimer = 0.0f;
		}
		if (titleScreenMusicVolume < 0) {
			titleScreenMusicVolume = 0;
		}
		titleScreenMusicVolumeTimer += deltaTime;
	}

	Mix_VolumeMusic(titleScreenMusicVolume);
	if (titleScreenAlpha >= 1.0f) {
		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		titleScreenMessageTimer += deltaTime;
		if (titleScreenMessageTimer > titleScreenMessageTime) {
			titleScreenMessageTimer = 0.0f;
		}
		if (titleScreenMessageTimer >= 1.0f) {
			mBatchRenderer.DrawInBatch(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 240.0f), mTextOut.mTextureSize * textSizeMultiplier, 
				static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[4]);
			}
			if (mActor.mEscaped) {
				mBatchRenderer.DrawInBatch(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 660.0f), mTextOut.mTextureSize * textSizeMultiplier,
					static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[1]);
		}
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush(mCamera.mUIModelMatrix);
	}

}

void App::ShutDown() {
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	mBatchRenderer.ShutDown();

	glDeleteProgram(mGraphicsPipelineShaderProgram);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

App& App::getInstance() {
	static App* app = new App;
	return *app;
}

void APIENTRY App::GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* param)
{
	const char* source_, * type_, * severity_;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             source_ = "API";             break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_ = "WINDOW_SYSTEM";   break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source_ = "SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     source_ = "THIRD_PARTY";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     source_ = "APPLICATION";     break;
	case GL_DEBUG_SOURCE_OTHER:           source_ = "OTHER";           break;
	default:                              source_ = "<SOURCE>";        break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               type_ = "ERROR";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_ = "DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_ = "UDEFINED_BEHAVIOR";   break;
	case GL_DEBUG_TYPE_PORTABILITY:         type_ = "PORTABILITY";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type_ = "PERFORMANCE";         break;
	case GL_DEBUG_TYPE_OTHER:               type_ = "OTHER";               break;
	case GL_DEBUG_TYPE_MARKER:              type_ = "MARKER";              break;
	default:                                type_ = "<TYPE>";              break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         severity_ = "HIGH";         break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severity_ = "MEDIUM";       break;
	case GL_DEBUG_SEVERITY_LOW:          severity_ = "LOW";          break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity_ = "NOTIFICATION"; break;
	default:                             severity_ = "<SEVERITY>";   break;
	}


	std::ostringstream stream;
	stream << "| Id: " << id << " | Severity: " << severity_ << " | Type: " << type_ << " | Source: (" << source_ << ") | Message: " << message << " |" << std::endl;
	std::string output = stream.str();

	std::string dashes(output.size() - 3, '-');

	if (static bool FistLineBoilerplate = true; FistLineBoilerplate) {
		std::cout << "" << std::endl;
		for (size_t i = 0; i < 3; i++) {
			std::cout << "(WARNING DEBUG MODE IS ENABLED AND MAY LEED TO PERFORMACNE ISSUES)\n" << std::endl;
		}
		std::cout << "|" << dashes << "|" << std::endl;
		FistLineBoilerplate = false;
	}

	std::cout << output << "|" << dashes << "|" << std::endl;

}
