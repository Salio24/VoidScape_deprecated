#include "App.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <algorithm>
#include "Sign.hpp"


App::App() {
	StartUp();
}

App::~App() {
	ShutDown();
}

void App::StartUp() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL2 could not initialize video subsystem" << std::endl;
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
	mBatchRenderer.StartUp();
	 
	mLevel.LoadLevel("levels/test_level.csv");
	mLevel.BuildLevel();

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		mLevel.mBlocks[i].Update();
	}

	mActor.mSprite.vertexData.Position = glm::vec2(800.0f, 180.0f);
	mActor.mSprite.vertexData.Size = glm::vec2(30.0f, 30.0f);

	mActor.mPosition = mActor.mSprite.vertexData.Position;

	test1.push_back(object1);

	mPipelineManager.CreateGraphicsPipeline();
}

void App::MainLoop() {
	SDL_WarpMouseInWindow(mWindow, mWindowWidth / 2, mWindowHeight / 2);
	//SDL_SetWindowRelativeMouseMode(mWindow, true);
	tp1 = std::chrono::system_clock::now();
	tp2 = std::chrono::system_clock::now();
	while (!mQuit) {
		mInputManager.Input();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

		//std::cout << "FPS: " << fps << std::endl;

		frameCount = 0;
		lastTime = currentTime;
	}
	//std::cout << deltaTime << std::endl;
	static int i = 0;
	if (i < 20) {
		deltaTimeBuffer += deltaTimeRaw;
	}
	if (i == 20) {
		//deltaTime = deltaTimeBuffer / 20;
		deltaTimeBuffer = 0;
		i = 0;
	}
	else {
		i++;
	}

	tp1 = tp2;
	mMovementHandler.Update(deltaTime);

	glUseProgram(mGraphicsPipelineShaderProgram);

	CollisionUpdate(mLevel.mBlocks, mActor, mMovementHandler.LeftWallHug, mMovementHandler.RightWallHug, deltaTime);

	mActor.Update();
	mCamera.Update(mActor.velocity, mActor.mScreenPosition, deltaTime);
	mActor.mScreenPosition = mCamera.GetProjectionMatrix() * glm::vec4(mActor.mPosition.x + mActor.mSprite.vertexData.Size.x / 2, mActor.mPosition.y + mActor.mSprite.vertexData.Size.y / 2, 0.0f, 1.0f);
	
	mBatchRenderer.BeginBatch();

	if (mActor.isWallMountableR && mMovementHandler.RightWallHug) {
		mBatchRenderer.DrawInBatch(glm::vec2(600.0f , 540.0f), glm::vec2(20.0f, 20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (mActor.isWallMountableL && mMovementHandler.LeftWallHug) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 540.0f), glm::vec2(20.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (mActor.isWallMounted) {
		mBatchRenderer.DrawInBatch(glm::vec2(640.0f, 540.0f), glm::vec2(20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(600.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::SPACE)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(640.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (mActor.isGrounded) {
		mBatchRenderer.DrawInBatch(glm::vec2(80.0f, 900.0f), glm::vec2(20.0f, 20.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush(mCamera.mUIModelMatrix);


	mBatchRenderer.BeginBatch();

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		//if (mLevel.mBlocks[i].mSprite.vertexData.Position.x < 330.0f) {
		mBatchRenderer.DrawInBatch(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, mLevel.mBlocks[i].mSprite.vertexData.Color);
		//}
	}

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush();

	

	mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, glm::vec3(1.0f, 0.0f, 1.0f), mActor.mModelMatrix);

	//std::cout << mActor.mPosition.x << std::endl;
	//static std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> tmp1;
	//if (int(mActor.mPosition.x) >= 1000.0f && tmpbool1 == true) {
	//	tmp1 = std::chrono::high_resolution_clock::now();
	//	tmpbool1 = false;
	//}
	//if (int(mActor.mPosition.x) >= 2200.0f && tmpbool2 == true) {
	//	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tmp1).count() << "!!!!!!!!!!!!!" << std::endl;
	//	tmpbool2 = false;
	//}
}

void App::ShutDown() {
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;

	mBatchRenderer.ShutDown();

	//glDeleteVertexArrays(1, &gMesh1.mVAO);
	glDeleteProgram(mGraphicsPipelineShaderProgram);

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
