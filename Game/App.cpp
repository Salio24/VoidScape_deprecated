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
	if (mDebug) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mWindow = SDL_CreateWindow("GL Window", mWindowWidth, mWindowHeight, SDL_WINDOW_OPENGL);

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
	if (mVsync == false) {
		SDL_GL_SetSwapInterval(0);
	}
	else {
		SDL_GL_SetSwapInterval(1);
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Glad was not initialized" << std::endl;
		exit(1);
	}
	if (mDebug) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void App::PostStartUp() {
	mCamera.SetProjectionMatrix();

	mPipelineManager.CreateGraphicsPipeline();
	glUseProgram(mGraphicsPipelineShaderProgram);

	mBatchRenderer.StartUp(mGraphicsPipelineShaderProgram);

	SDL_Surface* tileset = mTextureHandler.LoadSurface("assets/Level/tiles128up.png");
	
	mTextureHandler.InitTextureArray(GL_RGBA8, 512, 512, 1024);

	uint32_t whiteTextureData[128 * 128];
	for (int i = 0; i < 128 * 128; i++) {
		whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	}
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 128, 128, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.layersUsed[0]++;

	std::vector<SDL_Surface*> tiles = mTextureHandler.CutTileset(tileset, 128, 128);
	for (int i = 0; i < tiles.size(); i++) {
		mTextureHandler.LoadTexture(tiles[i], GL_RGBA, mTextureHandler.layersUsed[0], 0);
	}

	mAnimationHandler.Init(512);

	mTextOut.Init(mTextureHandler, "assets/Level/Text.png");

	Mix_AllocateChannels(16);
	mAudioHandler.LoadSounds();
}
void App::LoadGame() {
	mActor.velocity = glm::vec2(0.0f, 0.0f);

	mActor.mDead = false;
	mActor.isConsumedByVoid = false;
	mActor.isSucked = false;
	mActor.isSuckedPortal = false;
	mActor.mEscaped = false;
	mActor.flyAngle = 0.0f;
	mActor.flyAnglePortal = 0.0f;
	mActor.flyAngleTarget = -1.0f;
	mActor.flyAngleTargetPortal = -1.0f;
	mGameStarted = false;
	mBlackHole.isBorn = false;
	mBlackHole.loopTimerOneShot = false;
	mBlackHole.birthTimerOneShot = false;
	mBlackHole.idleVolume = 0;
	Mix_HaltChannel(13);
	mBlackHole.AABBSize.x = 100.0f;
	mLevel.mBlocks.clear();
	mMovementHandler.lookDirection = LookDirections::RIGHT;
	mCamera.mCameraOffset = glm::vec2(0.0f, 0.0f);
	mActor.mDeadSoundOneShot = true;
	mStateMachine.deadAnimOneShot = true;
	mStateMachine.deadAnimDone = false;
	Mix_HaltMusic();

	titleScreenAlpha = 0.0f;
	titleScreenMusicVolume = 128;

	titleScreenAlphaTimer = 0.0f;

	titleScreenMessageTimer = 0.0f;

	startMessageTimer = 0.0f;

	titleScreenMusicVolumeTimer = 0.0f;

	mLevel.LoadLevelJson("levels/GameLevels/32p/Level_1.json");
	mLevel.BuildLevel();

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		mLevel.mBlocks[i].Update();
	}
	mActor.mSprite.vertexData.Position = glm::vec2(370.0f, 350.0f);

	mActor.isVisible = true;
	mActor.isCollidable = true;

	mActor.mPosition = mActor.mSprite.vertexData.Position;

	mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;

	mEscapePortal.mSprite.vertexData.Size = mAnimationHandler.EscapePortalAnimation.Size * mEscapePortal.sizeMultiplier;

	Mix_PlayMusic(mAudioHandler.IntroMusic, 0);
}


void App::MainLoop() {
	SDL_WarpMouseInWindow(mWindow, mWindowWidth / 2, mWindowHeight / 2);
	tp1 = std::chrono::system_clock::now();
	tp2 = std::chrono::system_clock::now();
	while (!mQuit) {
		mInputManager.Input();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		Update();

		SDL_GL_SwapWindow(mWindow);  
	}

}

void App::Update() {
	static bool ad;
	tp2 = std::chrono::system_clock::now();
	static auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	static int frameCount = 0;
	frameCount++;
	// Our time per frame coefficient
	deltaTime = elapsedTime.count();
	if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1) {
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		auto fps = frameCount * 1000.0 / elapsedTime;

		std::cout << "FPS: " << fps << std::endl;

		frameCount = 0;
		lastTime = currentTime;
	}
	static int i = 0;
	if (i < 20) {
		deltaTimeBuffer += deltaTimeRaw;
	}
	if (i == 20) {
		deltaTimeBuffer = 0;
		i = 0;
	}
	else {
		i++;
	}

	tp1 = tp2;

	if (mGameStarted == false && mActor.velocity.x != 0.0f) {
		Mix_HaltMusic();
		mGameStarted = true;
	}

	if (mGameStarted) {
		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(mAudioHandler.LoopMusic, 0);
		}
	}
	else {
		startMessageTimer += deltaTime;
		if (startMessageTimer > startMessageTime) {
			startMessageTimer = 0.0f;
		}
		if (startMessageTimer >= 1.0f) {
			mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 470.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
				static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[5], mCamera.mUIModelMatrix);
		}
	}

	mMovementHandler.Update(deltaTime, mActor);

	mActor.Update();
	if (mGameStarted) {
		mBlackHole.Update(mLevel.mBlocks, mActor, deltaTime, mAnimationHandler.BlackHoleBirthAnimation, mAnimationHandler.BlackHoleLoopAnimation, mAudioHandler.BlackHoleBorn, mAudioHandler.ConsumedByVoid, mAudioHandler.BlackHoleIdle);
		mBatchRenderer.DrawSeperatly(mBlackHole.mSprite.vertexData.Position, mBlackHole.mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
			mBlackHole.mSprite.vertexData.TextureIndex, mBlackHole.AnimationSize, mBlackHole.mSprite.vertexData.TexturePosition, mBlackHole.mModelMatrix);
	}

	if (mActor.mPosition.y < -500.0f) {
		if (!mActor.mDead) {
			mActor.velocity = glm::vec2(0.0f, 0.0f);
			mActor.isConsumedByVoid = true;
			mActor.mDead = true;
		}
	}

	if (mActor.mDead) {
		if (mActor.mDeadSoundOneShot) {
			Mix_PlayChannel(14, mAudioHandler.FellDown, 0);
			mActor.mDeadSoundOneShot = false;
		}
	}


	CollisionUpdate(mLevel.mBlocks, mActor, mMovementHandler.LeftWallHug, mMovementHandler.RightWallHug, deltaTime, mMovementHandler.isGrounded);
	mCamera.Update(mActor.velocity, mActor.mScreenPosition, deltaTime);
	mActor.mScreenPosition = mCamera.GetProjectionMatrix() * glm::vec4(mActor.mPosition.x + mActor.mSprite.vertexData.Size.x / 2, mActor.mPosition.y + mActor.mSprite.vertexData.Size.y / 2, 0.0f, 1.0f);

	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());

	// buffer of block id that are being sucked into the black hole and visible
	std::vector<int> flyingBlocks;

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mLevel.mBlocks[i].isVisible == false) {
		}
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mLevel.mBlocks[i].isVisible == true && mLevel.mBlocks[i].isSucked == false) {
			mBatchRenderer.DrawInBatch(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, static_cast<uint32_t>(mLevel.mBlocks[i].mSprite.vertexData.TextureIndex), glm::vec2(0.25f, 0.25f));
		}
		else if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraOffset.x + 2000.0f) && mLevel.mBlocks[i].isVisible == true && mLevel.mBlocks[i].isSucked == true) {
			flyingBlocks.push_back(i);
		}
	}

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush();

	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush();

	mEscapePortal.Update(mAnimationHandler.EscapePortalAnimation, deltaTime, mActor, mAudioHandler.PortalEscape, mAudioHandler.PortalIdle);
	mBatchRenderer.DrawSeperatly(mEscapePortal.mSprite.vertexData.Position, mEscapePortal.mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
		mEscapePortal.mSprite.vertexData.TextureIndex, mEscapePortal.AnimationSize, mEscapePortal.mSprite.vertexData.TexturePosition, mEscapePortal.mModelMatrix);


	for (int i = 0; i < flyingBlocks.size(); i++) {
		mBatchRenderer.DrawSeperatly(mLevel.mBlocks[flyingBlocks[i]].mSprite.vertexData.Position, mLevel.mBlocks[flyingBlocks[i]].mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
			static_cast<uint32_t>(mLevel.mBlocks[flyingBlocks[i]].mSprite.vertexData.TextureIndex), glm::vec2(0.25f, 0.25f), glm::vec2(0.0f, 0.0f), mLevel.mBlocks[flyingBlocks[i]].mModelMatrix, false);
	}

	mStateMachine.Update(mMovementHandler, mAnimationHandler, mAudioHandler, mActor, deltaTime);
	if (mActor.isVisible == true) {
	mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mStateMachine.mCurrentActorDrawSize, mCamera.GetProjectionMatrix(),
		mStateMachine.mCurrentActorTextureIndex, mStateMachine.mCurrentActorTextureSize, mStateMachine.mCurrentActorTexturePosition, mActor.mModelMatrix, mStateMachine.mActorFlipped);	
	}

	TitleScreenUpdate();
}

void App::TitleScreenUpdate() {
	if (mActor.mDead || mActor.mEscaped || mActor.isConsumedByVoid) {
		mBatchRenderer.DrawSeperatly(glm::vec2(0.0f, 0.0f), glm::vec2(1920.0f, 1080.0f), glm::vec4((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), titleScreenAlpha), mCamera.GetProjectionMatrix(), mCamera.mUIModelMatrix);
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
		titleScreenMessageTimer += deltaTime;
		if (titleScreenMessageTimer > titleScreenMessageTime) {
			titleScreenMessageTimer = 0.0f;
		}
		if (titleScreenMessageTimer >= 1.0f) {
			mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 240.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
				static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[4], mCamera.mUIModelMatrix);
			}
			if (mActor.isConsumedByVoid && !mActor.mDead) {
				mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 660.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
					static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[0], mCamera.mUIModelMatrix);
			}
			else if (mActor.isConsumedByVoid && mActor.mDead) {
				mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 660.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
					static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[2], mCamera.mUIModelMatrix);
			}
			else if (!mActor.isConsumedByVoid && mActor.mDead) {
				mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 660.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
					static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[3], mCamera.mUIModelMatrix);
			}
			else if (mActor.mEscaped) {
				mBatchRenderer.DrawSeperatly(glm::vec2(960.0f - mTextOut.mTextureSize.x * textSizeMultiplier / 2, 660.0f), mTextOut.mTextureSize * textSizeMultiplier, mCamera.GetProjectionMatrix(),
					static_cast<uint32_t>(mTextOut.mTextTextureIndex), mTextOut.mTextureSize, mTextOut.mTexturePositions[1], mCamera.mUIModelMatrix);
		}
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

int App::GetWindowHeight() {
	return mWindowHeight;
}

int App::GetWindowWidth() {
	return mWindowWidth;
}

void App::SetGraphicsPipelineShaderProgram(GLuint program) {
	mGraphicsPipelineShaderProgram = program;
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
