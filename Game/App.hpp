#pragma once
#include <SDL3/SDL.h>
#include "Camera.hpp"
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include "IO.hpp"
#include "Input.hpp"
#include "PipelineManager.hpp"
#include "Sprite.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Actor.hpp"
#include "BatchRenderer.hpp"
#include "HelperStructs.hpp"
#include "GameLevel.hpp"
#include "CollisionHandler.hpp"
#include "MovementHandler.hpp"
#include <vector>
#include <chrono>
#include <SDL3/SDL_hints.h>
#include "Texture.hpp"
#include "AnimationHandler.hpp"
#include "AudioHandler.hpp"
#include "BlackHole.hpp"
#include "StateMachine.hpp"
#include "EscapePortal.hpp"
#include "SimpleTextOut.hpp"


class App {
	App();

	~App();

	App(const App&) = delete;

	App& operator=(const App&) = delete;

	void StartUp();

	void ShutDown();

	void Update();

	static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* param);
public:
	static App& getInstance();

	void PostStartUp();

	void LoadGame();
	
	void TitleScreenUpdate();

	void MainLoop();

	void SetGraphicsPipelineShaderProgram(GLuint program);

	int GetWindowWidth();
	int GetWindowHeight();

	int mWindowWidth{ 2560 };
	int mWindowHeight{ 1440 };

	SDL_Window* mWindow{ nullptr };
	SDL_GLContext mGlContext{ nullptr };

	GLuint mGraphicsPipelineShaderProgram{ 0 };

	InputManager mInputManager;

	BatchRenderer mBatchRenderer;

	Actor mActor;

	MovementHandler mMovementHandler;

	AudioHandler mAudioHandler;

	GameLevel mLevel;

	EscapePortal mEscapePortal;

	TextureHandler mTextureHandler;

	AnimationHandler mAnimationHandler;

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> tp1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> tp2;

	BlackHole mBlackHole;

	TextOut mTextOut;

	StateMachine mStateMachine;

	Camera mCamera;

	PipelineManager mPipelineManager;

	bool mGameStarted{ false };

	bool mQuit{ false };
	bool mVsync{ false };
	bool mDebug{ true };

	float deltaTime{ 0.1f };
	float deltaTimeRaw;
	float deltaTimeBuffer{ 0.0f };

	float textSizeMultiplier{ 800.0f };
	float titleScreenAlpha{ 0.0f };
	int titleScreenMusicVolume{ 128 };

	float titleScreenAlphaTime{ 0.01f };
	float titleScreenAlphaTimer{ 0.0f };

	float titleScreenMessageTime{ 2.0f };
	float titleScreenMessageTimer{ 0.0f };

	float titleScreenMusicVolumeTime{ 0.05f };
	float titleScreenMusicVolumeTimer{ 0.0f };

	float startMessageTime{ 2.0f };
	float startMessageTimer{ 0.0f };
};