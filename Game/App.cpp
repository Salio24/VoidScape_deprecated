#include "App.hpp"

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
			mSettings.currentResolutionIndex = 0;
		}
		else {
			mWindowHeight = 720;
			mWindowWidth = 1280;
			mSettings.currentResolutionIndex = 1;
		}
	}

	mSettings.screenSize.x = bounds.w;
	mSettings.screenSize.y = bounds.h;

	mResolutions.push_back(glm::ivec2(854, 480));
	mResolutions.push_back(glm::ivec2(1280, 720));
	mResolutions.push_back(glm::ivec2(1366, 768));
	mResolutions.push_back(glm::ivec2(1600, 900));
	mResolutions.push_back(glm::ivec2(1920, 1080));
	mResolutions.push_back(glm::ivec2(2560, 1440));
	mResolutions.push_back(glm::ivec2(3840, 2160));
	mResolutions.push_back(glm::ivec2(5120, 2880));
	mResolutions.push_back(glm::ivec2(7680, 4320));

	mWindowModes.push_back("windowed");
	mWindowModes.push_back("fullscreen");

	mSettings = JsonManager::loadSettings();
	mSaveData = JsonManager::loadSaveData();
	mInputManager.mControls = JsonManager::loadControls();

	mSettings.screenSize.x = bounds.w;
	mSettings.screenSize.y = bounds.h;

	if (Mix_OpenAudio(0, NULL) < 0)
	{
		std::cerr << "SDL3_mixer could not be initialized" << std::endl;
		exit(1);
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	if (mSettings.debugMode) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mWindow = SDL_CreateWindow("VoidScape", mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL);

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
	if (mSettings.VSync == false) {
		SDL_GL_SetSwapInterval(0);
	}
	else {
		SDL_GL_SetSwapInterval(1);
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Glad was not initialized" << std::endl;
		exit(1);
	}
	if (mSettings.debugMode) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}

	if (mWindowModes[mSettings.currentWindowModeIndex] == "windowed") {
		SDL_SetWindowSize(mWindow, mResolutions[mSettings.currentResolutionIndex].x, mResolutions[mSettings.currentResolutionIndex].y);
		mWindowWidth = mResolutions[mSettings.currentResolutionIndex].x;
		mWindowHeight = mResolutions[mSettings.currentResolutionIndex].y;

		mSettings.viewportOffset.x = 0;
		mSettings.viewportOffset.y = 0;

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		SDL_SetWindowFullscreen(mWindow, false);
	}
	else if (mWindowModes[mSettings.currentWindowModeIndex] == "fullscreen") {
		SDL_SetWindowFullscreen(mWindow, true);

		int Width;
		int Height;

		SDL_GetWindowSize(mWindow, &Width, &Height);

		glm::ivec2 resTemp = glm::ivec2(Width, Height);

		int i = 0;
		for (; mResolutions[i].y != mSettings.screenSize.y; i++) {

		}

		SDL_SetWindowSize(mWindow, resTemp.x, resTemp.y);
		mWindowWidth = resTemp.x;
		mWindowHeight = resTemp.y;

		mSettings.viewportOffset.x = (mSettings.screenSize.x - mWindowWidth) / 2;
		mSettings.viewportOffset.y = (mSettings.screenSize.y - mWindowHeight) / 2;

		glViewport(mSettings.viewportOffset.x, mSettings.viewportOffset.y, mWindowWidth, mWindowHeight);

	}
	if (mSettings.debugMode) {
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		if (__cplusplus == 202002L) std::cout << "C++20\n";
		else if (__cplusplus == 201703L) std::cout << "C++17\n";
		else if (__cplusplus == 201402L) std::cout << "C++14\n";
		else if (__cplusplus == 201103L) std::cout << "C++11\n";
		else if (__cplusplus == 199711L) std::cout << "C++98\n";
		else std::cout << "Unknown C++ standard\n";
	}
}

void App::PostStartUp() {

	mCamera.SetProjectionMatrix(1920.0f);

	mTextShaderProgram.CreateShaderProgram("shaders/vertexText.glsl", "shaders/fragmentText.glsl");
	mGeneralShaderProgram.CreateShaderProgram("shaders/vertexGeneral.glsl", "shaders/fragmentGeneral.glsl");
	mBackgroundShaderProgram.CreateShaderProgram("shaders/vertexBackground.glsl", "shaders/fragmentBackground.glsl");
	mBackgroundFramebufferShaderProgram.CreateShaderProgram("shaders/vertexBackgroundFramebuffer.glsl", "shaders/fragmentBackgroundFramebuffer.glsl");

	mTextRenderer.Init((const char*)"assets/Fonts/PixelOperator8-Bold.ttf", 96);

	mPipelineProgram.CreatePipelineProgram();
	mPipelineProgram.BindPipelineProgram();
	mPipelineProgram.BindShaderProgram(mGeneralShaderProgram.ID);

	mBatchRenderer.StartUp(&mGeneralShaderProgram, mPipelineProgram.ID);
	
	mTextureHandler.InitTextureArray(GL_RGBA8, 16, 16);
	mTextureHandler.InitTextureArray(GL_RGBA8, 32, 48);
	mTextureHandler.InitTextureArray(GL_RGBA8, 128, 600);
	mTextureHandler.InitTextureArray(GL_RGBA8, 512, 32);


	uint32_t whiteTextureData[16 * 16];
	for (int i = 0; i < 16 * 16; i++) {
		whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[16].first);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.mTextureArrays[16].second++;

	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[32].first);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.mTextureArrays[32].second++;

	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[128].first);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.mTextureArrays[128].second++;

	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[512].first);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.mTextureArrays[512].second++;

	mAnimationHandler.Init();

	Mix_AllocateChannels(20);
	mAudioHandler.LoadSounds();
	mEscapePortal.mSprite.mVertexData.Size = mAnimationHandler.EscapePortalAnimation.Size * mEscapePortal.mSizeMultiplier;
	if (mSettings.debugMode) {
		int maxCombinedUnits, maxFragmentUnits, maxVertexUnits, maxArrayLayers;

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedUnits);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentUnits);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexUnits);
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);

		printf("Max Combined Texture Units: %d\n", maxCombinedUnits);
		printf("Max Fragment Texture Units: %d\n", maxFragmentUnits);
		printf("Max Vertex Texture Units: %d\n", maxVertexUnits);
		printf("Max Array Texture Layers: %d\n", maxArrayLayers);
	}

	mTextureHandler.LoadAll();

	mSceneManager.mUIScenes.LoadMainMenuBackground("assets/UI/Data/Layout/MenuBorders.json", mTextureHandler.mTilesetLocations.mUIBorderTileset.second);

	mBackgroundRenderer.Init();

	mBackgroundRenderer.LoadMenuBackground("assets/UI/background4k.png", &mBackgroundShaderProgram, mPipelineProgram.ID, &mTextureHandler);

	mBackgroundRenderer.LoadLevelBackground(&mBackgroundShaderProgram, &mBackgroundFramebufferShaderProgram, mPipelineProgram.ID, &mTextureHandler);

	mBackgroundRenderer.ResizeFramebuffer(glm::ivec2(mWindowWidth, mWindowHeight));

	mAudioHandler.SetGlobalSFXVolume(mSettings.SFXVolume);
	mAudioHandler.SetGlobalMusicVolume(mSettings.MusicVolume);

	mSceneManager.Init(&mInputManager, &mBackgroundRenderer, &mAudioHandler, &mSettings, &mSaveData, &mBatchRenderer, &mTextRenderer);
}

void App::LoadGame(const bool retry) {
	mBlackHole.Reset();
	mStateMachine.Reset();
	mActor.Reset(mAnimationHandler.FallAnimation.Size, glm::vec2(400.0f, 350.0f));
	mSceneManager.mUIScenes.mTitleScreenActive = false;
	// reset movement handler
	mMovementHandler.mLookDirection = LookDirections::RIGHT;
	mSceneManager.mNewBestTimeMessageOneShot = false;

	// reset camera
	mCamera.mCameraOffset = glm::vec2(0.0f, 0.0f);

	mSceneManager.ResetTimer();

	// reset app
	gameStarted = false;
	startMessageTimer = 0.0f;
	mAudioHandler.mInitialMusicVolume = 128;
	mAudioHandler.SetGlobalMusicVolume(mSettings.MusicVolume);

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

		mSceneManager.mUIScenes.LoadMainMenuSettingsTab(mTextureHandler.mTilesetLocations.mUIBorderTileset.second, mTextureHandler.mTilesetLocations.mUIArrowsTileset.second);
		mSceneManager.mUIScenes.LoadMainMenuLevelsTab(mTextureHandler.mTilesetLocations.mUIBorderTileset.second, mSettings.debugMode);
		mSceneManager.mUIScenes.LoadTitleScreen(mTextureHandler.mTilesetLocations.mUIBorderTileset.second);
		mSceneManager.mUIScenes.LoadMainMenuControlsTab(mTextureHandler.mTilesetLocations.mUIBorderTileset.second);
		mSceneManager.LoadMainMenu(mTextureHandler.mTilesetLocations.mUIBorderTileset.second);
	}
	else if (retry) {
		mSceneManager.ReloadCurrentLevel(mTextureHandler.mTilesetLocations.mBaseTileset.second, mAudioHandler.IntroMusic);
	}

	mSceneManager.mUIScenes.LoadPauseMenu(mTextureHandler.mTilesetLocations.mUIBorderTileset.second);
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

void App::Update() {
	static int fps;
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
		fps = frameCount * 1000.0 / elapsedTime;

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

	if (mActor.mDead || mActor.mEscaped) {
		if (mAudioHandler.mMusicFadeoutTimer > mAudioHandler.mMusicFadeoutTime && mAudioHandler.mInitialMusicVolume > 0) {
			mAudioHandler.mInitialMusicVolume -= 1;
			mAudioHandler.mMusicFadeoutTimer = 0.0f;
		}
		if (mAudioHandler.mInitialMusicVolume < 0) {
			mAudioHandler.mInitialMusicVolume = 0;
		}
		mAudioHandler.mMusicFadeoutTimer += deltaTime;
		mAudioHandler.SetGlobalMusicVolume(mSettings.MusicVolume);
	}

	// game start logic
	if (gameStarted == false && mActor.mVelocity.x != 0.0f) {
		Mix_HaltMusic();
		gameStarted = true;
		mSceneManager.StartLevelTimer();
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
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, "Move to start", 900.0f, 500.0f, 0.5f, glm::vec3(0.80859375f, 0.80078125f, 0.81640625f), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
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
	if (gameStarted && !mPause && !mSettings.debugMode) {
		mBlackHole.Update(mSceneManager.mCurrentBlocks, mActor, deltaTime, mAnimationHandler.BlackHoleBirthAnimation, mAnimationHandler.BlackHoleLoopAnimation, mAudioHandler.BlackHoleBorn, mAudioHandler.ConsumedByVoid, mAudioHandler.BlackHoleIdle, mAudioHandler.mGlobalSFXVolumeModifier, mStateMachine.mActorDeathCause);
	}

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

	// level render vvv

	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[512].first);

	if (mSceneManager.mLevelActive == true) {
		if (gameStarted) {
			if (mBlackHole.mSprite.mVertexData.Position.x + mBlackHole.mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mBlackHole.mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
				mBatchRenderer.DrawInBatch(mBlackHole.mSprite.mVertexData.Position, mBlackHole.mSprite.mVertexData.Size, mBlackHole.mSprite.mVertexData.TextureIndex, mAnimationHandler.BlackHoleLoopAnimation.Size, mBlackHole.mSprite.mVertexData.TexturePosition);
			}
		}

		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();

		// buffer of block id that are being sucked into the black hole and visible
		std::vector<int> flyingBlocks;

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[mTextureHandler.mTilesetLocations.mBaseTileset.first].first);


		for (int i = 0; i < mSceneManager.mCurrentBlocks->size(); i++) {
			if (mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mSceneManager.mCurrentBlocks->at(i).mIsVisible == true && mSceneManager.mCurrentBlocks->at(i).mIsSucked == false) {
				mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.TextureIndex), glm::vec2(1.0f, 1.0f));
			}
			else if (mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mSceneManager.mCurrentBlocks->at(i).mIsVisible == true && mSceneManager.mCurrentBlocks->at(i).mIsSucked == true) {
				flyingBlocks.push_back(i);
			}
		}

		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[512].first);
		mEscapePortal.Update(mAnimationHandler.EscapePortalAnimation, deltaTime, mActor, mAudioHandler.PortalEscape, mAudioHandler.PortalIdle, mAudioHandler.mGlobalSFXVolumeModifier);
		if (mEscapePortal.mSprite.mVertexData.Position.x + mEscapePortal.mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
			&& mEscapePortal.mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
			mBatchRenderer.DrawInBatch(mEscapePortal.mSprite.mVertexData.Position, mEscapePortal.mSprite.mVertexData.Size, mEscapePortal.mSprite.mVertexData.TextureIndex, mAnimationHandler.EscapePortalAnimation.Size, mEscapePortal.mSprite.mVertexData.TexturePosition);
		}
		
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[mTextureHandler.mTilesetLocations.mBaseTileset.first].first);

		for (int i = 0; i < flyingBlocks.size(); i++) {
			if (mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position.x + mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Size.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
				&& mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f)) {
				mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mSprite.mVertexData.TextureIndex), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mFlyAngle, mSceneManager.mCurrentBlocks->at(flyingBlocks[i]).mFlyAngleOrientation);
			}
		}

		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();
	} // level render ^^^ menu render vvv
	else if (mSceneManager.mMainMenuActive == true) {
		for (const auto& pair : mSceneManager.mUIScenes.mButtonMap) {
			if (RectVsRect(glm::vec2(-10.0f, -10.0f), glm::vec2(2000.0f, 1160.0f), pair.second.mTriggerPos, pair.second.mTriggerSize)) {
				// idk
			}
			mSceneManager.mUIScenes.mButtonMap[pair.first].Render(&mBatchRenderer, mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix, mTextureHandler.mTextureArrays[mTextureHandler.mTilesetLocations.mUIBorderTileset.first].first);
		}
		if (mSceneManager.mMainMenuActive == true) {
			for (int i = 0; i < mSceneManager.mUIScenes.mTextToRender.size(); i++) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mTextToRender[i].Text, mSceneManager.mUIScenes.mTextToRender[i].Possition.x, mSceneManager.mUIScenes.mTextToRender[i].Possition.y, mSceneManager.mUIScenes.mTextToRender[i].Scale, mSceneManager.mUIScenes.mTextToRender[i].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mTextToRender[i].ModelMatrix, mSceneManager.mUIScenes.mTextToRender[i].CenteredX, mSceneManager.mUIScenes.mTextToRender[i].CenteredY, mSceneManager.mUIScenes.mTextToRender[i].RightSided);
			}
			for (auto& pair : mSceneManager.mUIScenes.mDynamicTextMap) {
				mTextRenderer.RenderText(&mTextShaderProgram, mPipelineProgram.ID, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Text, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Possition.x, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Possition.y, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Scale, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].Color, mCamera.GetProjectionMatrix(), *mSceneManager.mUIScenes.mDynamicTextMap[pair.first].ModelMatrix, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].CenteredX, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].CenteredY, mSceneManager.mUIScenes.mDynamicTextMap[pair.first].RightSided);
			}
		}

		mSceneManager.UpdateUIMenu(mTextureHandler.mTilesetLocations.mBaseTileset.second, deltaTime, mWindowWidth, mWindowHeight, mQuit, mWindowModes, mResolutions, mWindow);

		mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix(), &mCamera.mUIModelMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[mTextureHandler.mTilesetLocations.mUIBorderTileset.first].first);
		for (int i = 0; i < mSceneManager.mCurrentBlocks->size(); i++) {
			mBatchRenderer.DrawInBatch(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Position, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Size, static_cast<uint32_t>(mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.TextureIndex), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.0f, 1.0f, false, mSceneManager.mCurrentBlocks->at(i).mSprite.mVertexData.Color);
		}
		mBatchRenderer.EndBatch();
		mBatchRenderer.Flush();
	}

	// menu render ^^^

	// state machine update
	if (mSceneManager.mLevelActive && !mPause) {
		mStateMachine.Update(mMovementHandler, mAnimationHandler, mAudioHandler, mActor, deltaTime);
	}


	float angle = 0.0f;

	if (mStateMachine.mCurrentPlayerState != PlayerStates::RUNNING) {
		angle = 0.0f;
	}
	else {
		if (mStateMachine.mActorFlipped) {
			angle = glm::radians(mActor.mVelocity.x / 250.0f);
		}
		else {
			angle = glm::radians(mActor.mVelocity.x / 250.0f);
		}
	}
	mActor.mDoubleJumpOrb.Position = glm::vec2(mActor.mSprite.mVertexData.Position.x + (mStateMachine.mCurrentActorDrawSize.x / 2.0f) - (mActor.mDoubleJumpOrb.Size.x / 2.0f), mActor.mSprite.mVertexData.Position.y + mStateMachine.mCurrentActorDrawSize.y + 40.0f);

	// actor render
	if (mActor.mIsVisible == true && mSceneManager.mLevelActive == true) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureHandler.mTextureArrays[128].first);
		mBatchRenderer.DrawSeperatly(mCamera.GetProjectionMatrix(), mActor.mSprite.mVertexData.Position, mStateMachine.mCurrentActorDrawSize,
			mStateMachine.mCurrentActorTextureIndex, mStateMachine.mCurrentActorTextureSize, mStateMachine.mCurrentActorTexturePosition, angle, 1.0f, mStateMachine.mActorFlipped, &mActor.mModelMatrix);
		if (!mActor.mDead) {
			mBatchRenderer.DrawSeperatly(mCamera.GetProjectionMatrix(), mActor.mDoubleJumpOrb.Position, mActor.mDoubleJumpOrb.Size,
				mTextureHandler.mTilesetLocations.mDoubejumpOrbTileset.second + 1, glm::vec2(0.75f, 0.75f), mActor.mDoubleJumpOrb.TexturePosition, 0.0f, 1.0f, false, &mActor.mModelMatrix);
			if (mMovementHandler.mCanDoubleJump) {
				mBatchRenderer.DrawSeperatly(mCamera.GetProjectionMatrix(), mActor.mDoubleJumpOrb.Position, mActor.mDoubleJumpOrb.Size,
					mTextureHandler.mTilesetLocations.mDoubejumpOrbTileset.second, glm::vec2(0.75f, 0.75f), mActor.mDoubleJumpOrb.TexturePosition, 0.0f, 1.0f, false, &mActor.mModelMatrix);
			} 
		}
	}

	bool restart = false;
	bool restartMode = false;

	mSceneManager.UpdateUIInGame(restart, restartMode, mActor.mDead, mActor.mEscaped, mPause, gameStarted, mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix, deltaTime, mStateMachine.mActorDeathCause, &mTextShaderProgram, mPipelineProgram.ID, fps, mTextureHandler.mTextureArrays[mTextureHandler.mTilesetLocations.mUIBorderTileset.first].first);

	if (restart) {
		if (restartMode) {
			LoadGame();
		}
		else {
			LoadGame(true);
		}
	}
}

void App::ShutDown() {
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	mBatchRenderer.ShutDown();

	glDeleteProgram(mGraphicsPipelineShaderProgram);

	Mix_Quit();
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
