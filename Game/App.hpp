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
	void MainLoop();

	void SetGraphicsPipelineShaderProgram(GLuint program);

	int GetWindowWidth();
	int GetWindowHeight();

	int mWindowWidth{ 1920 };
	int mWindowHeight{ 1080 };

	SDL_Window* mWindow{ nullptr };
	SDL_GLContext mGlContext{ nullptr };

	GLuint mGraphicsPipelineShaderProgram{ 0 };

	glm::vec2 pos2;

	InputManager mInputManager;

	bool gravity{ true };

	BatchRenderer mBatchRenderer;

	Actor mActor;

	MovementHandler mMovementHandler;

	AudioHandler mAudioHandler;

	GameLevel mLevel;

	TextureHandler mTextureHandler;

	AnimationHandler mAnimationHandler;

	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> tp1;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1, 10000000>>> tp2;

	GameEntity object1;

	glm::vec4 temp_color;

	std::vector<GameObject> test1;

	Camera mCamera;

	PipelineManager mPipelineManager;

	bool tmpbool1{ true };
	bool tmpbool2{ true };

	bool mQuit{ false };
	bool mVsync{ true };
	bool mDebug{ true };

	float deltaTime{ 0.1f };
	float deltaTimeRaw;
	float deltaTimeBuffer{ 0.0f };
};