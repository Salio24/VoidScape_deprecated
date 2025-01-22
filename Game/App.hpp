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
#include "ShaderProgram.hpp"
#include "PipelineProgram.hpp"
#include "TextRenderer.hpp"
#include "SceneManager.hpp"
#include "LevelScene.hpp"
#include "UIScenes.hpp"
#include "BackgroundRenderer.hpp"

class App {
public:
	static App& getInstance();

	void PostStartUp();

	void LoadGame(const bool retry = false);

	void MainLoop();

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

	BlackHole mBlackHole;

	TextOut mTextOut;

	StateMachine mStateMachine;

	Camera mCamera;


	PipelineManager mPipelineManager;

	ShaderProgram mGeneralShaderProgram;
	ShaderProgram mTextShaderProgram;
	ShaderProgram mBackgroundShaderProgram;
	ShaderProgram mBackgroundFramebufferShaderProgram;


	PipelineProgram mPipelineProgram;

	SceneManager mSceneManager;

	BackgroundRenderer mBackgroundRenderer;

	TextRenderer mTextRenderer;

	bool mQuit{ false };
	bool mPause{ false };

	glm::ivec2 viewportOffset{ 0 };

	int mWindowWidth{ 1920 };
	int mWindowHeight{ 1080 };
private: 
	App();

	~App();

	App(const App&) = delete;

	App& operator=(const App&) = delete;

	void StartUp();

	void ShutDown();

	void Update();

	void UpdatePlayground();

	void UIUpdate();

	static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* param);

	bool vsync      { false };
	bool debugMode  { true };
	bool gameStarted{ false };
	
	bool autoRestart{ false };

	int SFXVolume{ 3 };
	int MusicVolume{ 3 };

	float deltaTimeRaw;
	float deltaTime      { 0.1f };
	float deltaTimeBuffer{ 0.0f };

	float textSizeMultiplier    { 800.0f };
	float titleScreenAlpha      { 0.0f };
	int   titleScreenMusicVolume{ 128 };

	float titleScreenAlphaTime { 0.01f };
	float titleScreenAlphaTimer{ 0.0f };
	bool titleScreenDarkened{ false };

	float titleScreenMessageTime { 2.0f };
	float titleScreenMessageTimer{ 0.0f };

	float titleScreenMusicVolumeTime { 0.05f };
	float titleScreenMusicVolumeTimer{ 0.0f };

	float startMessageTime { 2.0f };
	float startMessageTimer{ 0.0f };

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint2;

	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> levelTimerTotal;
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> pauseTimer;

	long long levelTimeTotal{ 0 };
	long long levelTime{ 0 };
	long long pauseTime{ 0 };

	int levelTime_minutes{ 0 };
	int levelTime_seconds{ 0 };
	int levelTime_centiseconds{ 0 };

	int best_levelTime_minutes{ 0 };
	int best_levelTime_seconds{ 0 };
	int best_levelTime_centiseconds{ 0 };

	bool newBestTimeMessageOneShot{ false };

	SDL_Window* mWindow     { nullptr };
	SDL_GLContext mGlContext{ nullptr };

	glm::ivec2 screenSize{ 0 };

	std::vector<glm::ivec2> resolutions;

	int currentResolutionIndex{ 4 };

	std::vector<std::string> windowModes;

	int currentWindowModeIndex{ 0 };
};