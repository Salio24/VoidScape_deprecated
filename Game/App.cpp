#include "App.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <algorithm>
#include "Sign.hpp"
#include <cmath>
#include <SDL3/SDL_image.h>

GLuint textureID1{1};
GLuint textureID2{2};


App::App() {
	StartUp();
}

App::~App() {
	ShutDown();
}

void App::StartUp() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL3 could not initialize video subsystem" << std::endl;
		exit(1);
	}

	if (IMG_Init(IMG_INIT_PNG) == 0) {
		std::cerr << "SDL3_Image could not be initialized" << std::endl;
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	if (mDebug) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
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

	//auto location = glGetUniformLocation(mGraphicsPipelineShaderProgram, "uTextures");
	//int samplers[32];
	//for (int i = 0; i < 32; i++) {
	//	samplers[i] = i;
	//}
	//glUniform1iv(location, 32, samplers);

	mBatchRenderer.StartUp(mGraphicsPipelineShaderProgram);
	


	//mLevel.LoadLevel("levels/test_level.csv");
	mLevel.LoadLevel("levels/GameLevels/Proto_Level_BaseLayer.csv");
	mLevel.BuildLevel();

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		mLevel.mBlocks[i].Update();
	}

	mActor.mSprite.vertexData.Position = glm::vec2(800.0f, 280.0f);
	mActor.mSprite.vertexData.Size = glm::vec2(30.0f, 30.0f);

	mActor.mPosition = mActor.mSprite.vertexData.Position;

	test1.push_back(object1);

	SDL_Surface* tileset = mTextureHandler.LoadSurface("levels/GameLevels/tiles.png");

	//IMG_SavePNG(tileset, "tiles.png");

	mTextureHandler.InitTextureArray(GL_RGBA8, 8, 8, 1024);

	std::vector<SDL_Surface*> tiles = mTextureHandler.CutTileset(tileset, 8, 8);

	uint32_t whiteTextureData[8 * 8];
	for (int i = 0; i < 8 * 8; i++) {
		whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	}
	
	
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 8, 8, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);

	for (int i = 0; i < tiles.size(); i++) {
		mTextureHandler.LoadTexture(tiles[i], GL_RGBA, i + 1);
	}

	//IMG_SavePNG(tiles[0], "tile.png");

	// temp

	//SDL_Surface* surface = IMG_Load("levels/tiles/321.png");
	//if (!surface) {
	//	std::cerr << "IMG_Load Error: " << std::endl;
	//	exit(1);
	//}
	//SDL_Surface* surface1 = IMG_Load("levels/tiles/122.png");
	//if (!surface1) {
	//	std::cerr << "IMG_Load Error: " << std::endl;
	//	exit(1);
	//}
	//glGenTextures(1, &textureArray);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
	//
	//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 8, 8, 1024);
	//
	//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, 8, 8, 1, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, 8, 8, 1, GL_RGBA, GL_UNSIGNED_BYTE, surface1->pixels);
	//
	//uint32_t whiteTextureData[8 * 8];
	//for (int i = 0; i < 8 * 8; i++) {
	//	whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	//}
	//
	//
	//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 8, 8, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	//
	//// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//
	//glActiveTexture(GL_TEXTURE0);  // Ensure we're working on texture unit 0
	//glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);  // Bind the texture array
	//glUniform1i(glGetUniformLocation(mGraphicsPipelineShaderProgram, "uTextures"), 0); // Set sampler uniform


	int maxCombinedUnits, maxFragmentUnits, maxVertexUnits, maxArrayLayers;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedUnits);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentUnits);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);

	printf("Max Combined Texture Units: %d\n", maxCombinedUnits);
	printf("Max Fragment Texture Units: %d\n", maxFragmentUnits);
	printf("Max Vertex Texture Units: %d\n", maxVertexUnits);
	printf("Max Array Texture Layers: %d\n", maxArrayLayers);

	int activeTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTextureUnit);
	printf("Active texture unit: GL_TEXTURE%d\n", activeTextureUnit - GL_TEXTURE0);

	int boundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &boundTexture);
	printf("Bound texture ID to GL_TEXTURE0: %d\n", boundTexture);

	int samplerValue;
	int location = glGetUniformLocation(mGraphicsPipelineShaderProgram, "uTextures");
	glGetUniformiv(mGraphicsPipelineShaderProgram, location, &samplerValue);
	printf("Uniform 'ourTextures' is set to texture unit: %d\n", samplerValue);

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
		glClearColor((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 1.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	CollisionUpdate(mLevel.mBlocks, mActor, mMovementHandler.LeftWallHug, mMovementHandler.RightWallHug, deltaTime);

	mActor.Update();
	mCamera.Update(mActor.velocity, mActor.mScreenPosition, deltaTime);
	mActor.mScreenPosition = mCamera.GetProjectionMatrix() * glm::vec4(mActor.mPosition.x + mActor.mSprite.vertexData.Size.x / 2, mActor.mPosition.y + mActor.mSprite.vertexData.Size.y / 2, 0.0f, 1.0f);



	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());

	if (mActor.isWallMountableR && mMovementHandler.RightWallHug) {
		mBatchRenderer.DrawInBatch(glm::vec2(600.0f , 540.0f), glm::vec2(20.0f, 20.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	if (mActor.isWallMountableL && mMovementHandler.LeftWallHug) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 540.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(600.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::SPACE)]) {
		//mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		//mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 810.0f), glm::vec2(20.0f, 20.0f), textureID1);
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(640.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_UP)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 830.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 790.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	}
	if (mMovementHandler.KeyboadStates[static_cast<int>(MovementState::DUCK)]) {
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 750.0f), glm::vec2(20.0f, 20.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	if (mActor.isGrounded) {
		mBatchRenderer.DrawInBatch(glm::vec2(80.0f, 900.0f), glm::vec2(20.0f, 20.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush(mCamera.mUIModelMatrix);

	uint32_t ddd = 0;


	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
	//mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 1000.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.53f, 0.12f, 0.87f, 1.0f));
	uint32_t d = 0;
	for (int u = 0; u < 23; u++) {
		d++;
		//mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 850.0f), glm::vec2(10.0f + 10.0f * d, 10.0f), d);
	}
	//for (uint32_t tt = 0; tt < 23; tt++) {
	//	mBatchRenderer.DrawInBatch(glm::vec2(620.0f + 40.0f * tt, 850.0f), glm::vec2(40.0f, 40.0f), tt);
	//}

	mBatchRenderer.DrawInBatch(glm::vec2(80.0f, 800.0f), glm::vec2(40.0f, 40.0f), ddd);


	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush(mCamera.mUIModelMatrix);




	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
	int j = 0;

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x - 80.0f) 
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x + 2000.0f)) {
		mBatchRenderer.DrawInBatch(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, static_cast<uint32_t>(mLevel.mBlocks[i].mSprite.vertexData.TextureIndex));
		j++;
		}
	}

	//std::cout << j << std::endl;

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush();

	glm::vec4 temp_color;

	if (!mActor.isGrounded && !mMovementHandler.canWallStick && mMovementHandler.canDoubleJump) {
		temp_color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (!mActor.isGrounded && !mMovementHandler.canWallStick && !mMovementHandler.canDoubleJump) {
		temp_color = glm::vec4(0.5f, 0.0f, 1.0f, 1.0f);
	}
	else if (!mActor.isGrounded && mMovementHandler.canWallStick) {
		temp_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	else {
		temp_color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}

	if (mMovementHandler.isSliding) {
		temp_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, temp_color, mCamera.GetProjectionMatrix(), mActor.mModelMatrix);

	//std::cout << "Km/h :" << std::abs((mActor.velocity.x / 54.0f) * 3.6f) << std::endl;
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
