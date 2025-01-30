#pragma once
#include <SDL3/SDL.h>
#include "Camera.hpp"
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include "Input.hpp"
#include "Sprite.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Actor.hpp"
#include "BatchRenderer.hpp"
#include "HelperStructs.hpp"
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
#include "JsonFileManager.hpp"
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
#include <numbers>

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

	EscapePortal mEscapePortal;

	TextureHandler mTextureHandler;

	AnimationHandler mAnimationHandler;

	BlackHole mBlackHole;

	StateMachine mStateMachine;

	Camera mCamera;

	ShaderProgram mGeneralShaderProgram;
	ShaderProgram mTextShaderProgram;
	ShaderProgram mBackgroundShaderProgram;
	ShaderProgram mBackgroundFramebufferShaderProgram; 

	PipelineProgram mPipelineProgram;

	SceneManager mSceneManager;

	BackgroundRenderer  mBackgroundRenderer;

	TextRenderer mTextRenderer;

	JsonManager mJsonManager;

	bool mQuit{ false };
	bool mPause{ false };

	Settings mSettings;

	SaveData mSaveData;

	int mWindowWidth{ 1920 };
	int mWindowHeight{ 1080 };

	std::vector<std::string> mWindowModes;

	std::vector<glm::ivec2> mResolutions;

private: 
	App();

	~App();

	App(const App&) = delete;

	App& operator=(const App&) = delete;

	void StartUp();

	void ShutDown();

	void Update();

	void UpdatePlayground();

	static void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* param);

	bool gameStarted{ false };

	float deltaTimeRaw;
	float deltaTime      { 0.1f };
	float deltaTimeBuffer{ 0.0f };

	float textSizeMultiplier    { 800.0f };

	float startMessageTime { 2.0f };
	float startMessageTimer{ 0.0f };

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> TimePoint2;

	SDL_Window* mWindow     { nullptr };
	SDL_GLContext mGlContext{ nullptr };

};