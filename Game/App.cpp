#include "App.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cmath>
#include <algorithm>
#include "Sign.hpp"
#include <cmath>
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

	mBatchRenderer.StartUp(mGraphicsPipelineShaderProgram);
	mLevel.LoadLevelJson("levels/GameLevels/32p/Level_1.json");
	mLevel.BuildLevel();

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		mLevel.mBlocks[i].Update();
	}

	mActor.mSprite.vertexData.Position = glm::vec2(12000.0f, 970.0f);
	mActor.mSprite.vertexData.Size = glm::vec2(25.0f, 32.0f);

	mActor.mPosition = mActor.mSprite.vertexData.Position;

	SDL_Surface* tileset = mTextureHandler.LoadSurface("assets/Level/tiles32.png");

	mTextureHandler.InitTextureArray(GL_RGBA8, 32, 32, 1024);

	uint32_t whiteTextureData[32 * 32];
	for (int i = 0; i < 32 * 32; i++) {
		whiteTextureData[i] = 0xFFFFFFFF; // RGBA: White
	}
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 32, 32, 1, GL_RGBA, GL_UNSIGNED_BYTE, whiteTextureData);
	mTextureHandler.layersUsed[0]++;

	std::vector<SDL_Surface*> tiles = mTextureHandler.CutTileset(tileset, 32, 32);
	for (int i = 0; i < tiles.size(); i++) {
		mTextureHandler.LoadTexture(tiles[i], GL_RGBA, mTextureHandler.layersUsed[0], 0);
		//std::cout << "Loading texture into Texture2DArray on layer:" << i + 1 << std::endl;
	}

	mAnimationHandler.Init();

	mAudioHandler.LoadSounds();

	int maxCombinedUnits, maxFragmentUnits, maxVertexUnits, maxArrayLayers;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedUnits);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentUnits);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexUnits);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);

	//printf("Max Combined Texture Units: %d\n", maxCombinedUnits);
	//printf("Max Fragment Texture Units: %d\n", maxFragmentUnits);
	//printf("Max Vertex Texture Units: %d\n", maxVertexUnits);
	//printf("Max Array Texture Layers: %d\n", maxArrayLayers);

	int activeTextureUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTextureUnit);
	//printf("Active texture unit: GL_TEXTURE%d\n", activeTextureUnit - GL_TEXTURE0);

	int boundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &boundTexture);
	//printf("Bound texture ID to GL_TEXTURE0: %d\n", boundTexture);

	int samplerValue;
	int location = glGetUniformLocation(mGraphicsPipelineShaderProgram, "uTextures");
	glGetUniformiv(mGraphicsPipelineShaderProgram, location, &samplerValue);
	//printf("Uniform 'ourTextures' is set to texture unit: %d\n", samplerValue);
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor((14.0f / 256.0f), (7.0f / 256.0f), (27.0f / 256.0f), 1.0f);
		//glClearColor((18.0f / 256.0f), (14.0f / 256.0f), (40.0f / 256.0f), 1.0f);
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

	CollisionUpdate(mLevel.mBlocks, mActor, mMovementHandler.LeftWallHug, mMovementHandler.RightWallHug, deltaTime, mMovementHandler.isGrounded);

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
		mBatchRenderer.DrawInBatch(glm::vec2(620.0f, 810.0f), glm::vec2(20.0f, 20.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
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
	if (mMovementHandler.isGrounded) {
		mBatchRenderer.DrawInBatch(glm::vec2(80.0f, 900.0f), glm::vec2(20.0f, 20.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush(mCamera.mUIModelMatrix);


	mBatchRenderer.BeginBatch(mCamera.GetProjectionMatrix());
	int j = 0;

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x + 2000.0f) && mLevel.mBlocks[i].isVisible == false) {
			//mBatchRenderer.DrawInBatch(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, glm::vec4(1.0f, 0.0f, 0.0f, 0.2f));
		}
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x + 2000.0f) && mLevel.mBlocks[i].isVisible == true && mLevel.mBlocks[i].isSucked == false) {
		mBatchRenderer.DrawInBatch(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, static_cast<uint32_t>(mLevel.mBlocks[i].mSprite.vertexData.TextureIndex));
		j++;
		}
	}

	//std::cout << j << std::endl;

	mBatchRenderer.EndBatch();
	mBatchRenderer.Flush();

	glm::vec4 temp_color1 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

	//mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, temp_color, mCamera.GetProjectionMatrix(), mActor.mModelMatrix);

	//sound vvv



	//sound ^^^



	//black hole code vvv
	// needs refactoring and optimization, this is just a prototype version
	
	glm::vec2 voidPos = glm::vec2(200.0f, 500.0f);
	static glm::vec2 range = glm::vec2(100.0f, 1080.f);

	//range.x += 144.0f * deltaTime;

	//mBatchRenderer.DrawSeperatly(glm::vec2(0.0f, 0.0f), range, glm::vec4(1.0f, 1.0f, 1.0f, 0.02f), mCamera.GetProjectionMatrix());
	mBatchRenderer.DrawSeperatly(voidPos, glm::vec2(40.0f, 40.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), mCamera.GetProjectionMatrix());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(1.0f, 5.0f);

	if (RectVsRect(glm::vec2(0.0f, 0.0f), range, mActor.mPosition, mActor.mSprite.vertexData.Size) && mActor.isSucked == false) {
		//mActor.isSucked = true;
		//mMovementHandler.debugMove = true;
		//mActor.flyDirectionNormalized = glm::normalize(voidPos - mActor.mPosition);
		//std::cout << glm::to_string(mActor.flyDirectionNormalized) << std::endl;
		//mActor.velocity = mActor.flyDirectionNormalized * (float)dis(gen);
	}

	std::vector<std::pair<int, float>> affectedBlocks;

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (RectVsRect(glm::vec2(0.0f, 0.0f), range, mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size) && mLevel.mBlocks[i].isVisible == true && mLevel.mBlocks[i].isSucked == false) {
			affectedBlocks.push_back({i , glm::distance2(voidPos, mLevel.mBlocks[i].mSprite.vertexData.Position)});
		}
	}

	std::sort(affectedBlocks.begin(), affectedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
		return a.second < b.second;
		});

	if (mActor.isSucked == true) {
		mActor.velocity += mActor.velocity * 2.16f * deltaTime;

		glm::vec2 center = glm::vec2(mActor.mSprite.vertexData.Position.x + mActor.mSprite.vertexData.Size.x / 2, mActor.mSprite.vertexData.Position.y + mActor.mSprite.vertexData.Size.y / 2);

		// Compute the dot product
		float dot = glm::dot(mActor.flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		// Compute the magnitudes of both vectors
		float magV1 = glm::length(glm::vec2(-1.0f, 0.0f));
		float magV2 = glm::length(mActor.flyDirectionNormalized);

		// Compute the cosine of the angle
		float cosTheta = dot / (magV1 * magV2);

		// Clamp the value to avoid numerical errors leading to invalid values for acos
		cosTheta = glm::clamp(cosTheta, -1.0f, 1.0f);

		// Calculate the angle in radians
		mActor.flyAngleTarget = glm::acos(cosTheta);


		// Step 1: Translate to origin
		mActor.mModelMatrix = glm::translate(mActor.mModelMatrix, glm::vec3(center, 0.0f));

		float crossProduct = glm::cross(mActor.flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		if (crossProduct > 0) {
			mActor.mModelMatrix = glm::rotate(mActor.mModelMatrix, -mActor.flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (crossProduct < 0) {
			mActor.mModelMatrix = glm::rotate(mActor.mModelMatrix, mActor.flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		// Step 2: Apply rotation
		
		// Step 3: Translate back
		mActor.mModelMatrix = glm::translate(mActor.mModelMatrix, glm::vec3(-center, 0.0f));
	}

	if (mActor.flyAngle < mActor.flyAngleTarget) {
		mActor.flyAngle += 0.1f * deltaTime;
	}

	for (int i = 0; i < affectedBlocks.size(); i++) {
		mLevel.mBlocks[affectedBlocks[i].first].isVisible = true;
		mLevel.mBlocks[affectedBlocks[i].first].isCollidable = false;
		mLevel.mBlocks[affectedBlocks[i].first].isSucked = true;
		mLevel.mBlocks[affectedBlocks[i].first].flyDirectionNormalized = glm::normalize(voidPos - mLevel.mBlocks[affectedBlocks[i].first].mSprite.vertexData.Position);
		mLevel.mBlocks[affectedBlocks[i].first].tempVelocity = mLevel.mBlocks[affectedBlocks[i].first].flyDirectionNormalized * (float)dis(gen);
	}

	float voidTime = 0.01f;
	static float voidTimer = 0.0f;
	static int index1 = 0;
	static int rot = 0;

	voidTimer += deltaTime;

	if (voidTimer > voidTime) {
		rot += 1;


		voidTimer = 0.0f;
	}

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (mLevel.mBlocks[i].isSucked == true && mLevel.mBlocks[i].isVisible == true) {
			mLevel.mBlocks[i].isCollidable = false;
			mLevel.mBlocks[i].mSprite.vertexData.Position = mLevel.mBlocks[i].mSprite.vertexData.Position + mLevel.mBlocks[i].tempVelocity * deltaTime;
			mLevel.mBlocks[i].tempVelocity += mLevel.mBlocks[i].tempVelocity * 2.16f * deltaTime;
			// may need to use dynamic rectvsrect
			if (RectVsRect(glm::vec2(voidPos.x, voidPos.y), glm::vec2(40.0f, 40.0f), mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size)) {
				mLevel.mBlocks[i].isVisible = false;
			}
		}
	}

	std::uniform_real_distribution<> dis2(10.0f, 30.0f);

	for (int i = 0; i < mLevel.mBlocks.size(); i++) {
		if (mLevel.mBlocks[i].mSprite.vertexData.Position.x > (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x - 80.0f)
			&& mLevel.mBlocks[i].mSprite.vertexData.Position.x < (mActor.mPosition.x - 800.0f + mCamera.mCameraVelocity.x + 2000.0f) && mLevel.mBlocks[i].isVisible == true && mLevel.mBlocks[i].isSucked == true) {


			glm::mat4 model = glm::mat4(1.0f);

			glm::vec2 center = glm::vec2(mLevel.mBlocks[i].mSprite.vertexData.Position.x + mLevel.mBlocks[i].mSprite.vertexData.Size.x / 2, mLevel.mBlocks[i].mSprite.vertexData.Position.y + mLevel.mBlocks[i].mSprite.vertexData.Size.y / 2);

			// Compute the dot product
			float dot = glm::dot(mLevel.mBlocks[i].flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

			// Compute the magnitudes of both vectors
			float magV1 = glm::length(glm::vec2(-1.0f, 0.0f));
			float magV2 = glm::length(mLevel.mBlocks[i].flyDirectionNormalized);

			// Compute the cosine of the angle
			float cosTheta = dot / (magV1 * magV2);

			// Clamp the value to avoid numerical errors leading to invalid values for acos
			cosTheta = glm::clamp(cosTheta, -1.0f, 1.0f);

			// Calculate the angle in radians
			mLevel.mBlocks[i].flyAngleTarget = glm::acos(cosTheta);


			// Step 1: Translate to origin
			model = glm::translate(model, glm::vec3(center, 0.0f));

			float crossProduct = glm::cross(mLevel.mBlocks[i].flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

			if (crossProduct > 0) {
				model = glm::rotate(model, -mLevel.mBlocks[i].flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (crossProduct < 0) {
				model = glm::rotate(model, mLevel.mBlocks[i].flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			// Step 2: Apply rotation

			// Step 3: Translate back
			model = glm::translate(model, glm::vec3(-center, 0.0f));

			if (mLevel.mBlocks[i].flyAngle < mLevel.mBlocks[i].flyAngleTarget) {
				mLevel.mBlocks[i].flyAngle += 1.0f * deltaTime;
			}


			mBatchRenderer.DrawSeperatly(mLevel.mBlocks[i].mSprite.vertexData.Position, mLevel.mBlocks[i].mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
				static_cast<uint32_t>(mLevel.mBlocks[i].mSprite.vertexData.TextureIndex), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), model, false);
		}
	}

	if (rot == 360) {
		rot = 0;
	}


	//black hole code ^^^

	

	bool flipped;

	switch (mMovementHandler.lookDirection) {
	case LookDirections::LEFT:
		flipped = true;
		break;
	case LookDirections::RIGHT:
		flipped = false;
		break;
	}
	//std::cout << mAnimationHandler.RunAnimation.index << std::endl;


	static bool runAnimOneShot = true;
	static bool runsound;
	static float FallVolumeTime = 0.1f;
	static float FallVolumeTimer = 0.0f;
	static int FallVolume = 1;
	switch (mMovementHandler.currentPlayerState) {
	case PlayerStates::RUNNING:
		if (runAnimOneShot) {
			mAnimationHandler.RunAnimation.AnimationTimer = std::chrono::high_resolution_clock::now();
			mAnimationHandler.RunAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			runAnimOneShot = false;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mAnimationHandler.RunAnimation.AnimationTimer).count() > mAnimationHandler.RunAnimation.AnimationTime + deltaTime * 1000) {
			mAnimationHandler.RunAnimation.AnimationTimer = std::chrono::high_resolution_clock::now();
			mAnimationHandler.RunAnimation.index = 0;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - mAnimationHandler.RunAnimation.SingleFrameTimer).count() > mAnimationHandler.RunAnimation.SingleFrameTime + deltaTime * 1000) {
			mAnimationHandler.RunAnimation.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			mAnimationHandler.RunAnimation.index++;
		}
		if (mAnimationHandler.RunAnimation.index > mAnimationHandler.RunAnimation.AnimationTextureIndexes.size() - 1) {
			mAnimationHandler.RunAnimation.index = 0;
		}
		if ((mAnimationHandler.RunAnimation.index == 1 || mAnimationHandler.RunAnimation.index == 2 || mAnimationHandler.RunAnimation.index == 4 || mAnimationHandler.RunAnimation.index == 5) && runsound == false) {
			runsound = true;
		}

		//std::cout << "runsound: " << runsound << std::endl;

		if ((mAnimationHandler.RunAnimation.index == 3 || mAnimationHandler.RunAnimation.index == 0) && runsound == true) {
			runsound = false;
			mAudioHandler.PlayNextStepSound();
		}

		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.RunAnimation.Size* mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.RunAnimation.AnimationTextureIndexes[mAnimationHandler.RunAnimation.index], mAnimationHandler.RunAnimation.Size, mAnimationHandler.RunAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		//std::cout << "RUN" << std::endl;
		break;
	case PlayerStates::JUMPING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		//mActor.mSprite.vertexData.Size = mAnimationHandler.JumpAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.JumpAnimation.Size * mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.JumpAnimation.AnimationTextureIndexes[0], mAnimationHandler.JumpAnimation.Size, mAnimationHandler.JumpAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		if (mMovementHandler.CheckPlayerStateChange()) {
			Mix_PlayChannel(2, mAudioHandler.Jump, 0);
		}
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::DOUBLE_JUMPING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		//mActor.mSprite.vertexData.Size = mAnimationHandler.JumpAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.JumpAnimation.Size * mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.JumpAnimation.AnimationTextureIndexes[0], mAnimationHandler.JumpAnimation.Size, mAnimationHandler.JumpAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		//std::cout << "JUMP" << std::endl;
		if (mMovementHandler.CheckPlayerStateChange()) {
			Mix_PlayChannel(3, mAudioHandler.DoubleJump, 0);
		}
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::FALLING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		//mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.FallAnimation.Size* mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.FallAnimation.AnimationTextureIndexes[0], mAnimationHandler.FallAnimation.Size, mAnimationHandler.FallAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		if (!Mix_Playing(4)) {
			Mix_PlayChannel(4, mAudioHandler.WindSoft, 0);
		}

		if (FallVolumeTimer >= FallVolumeTime) {
			FallVolumeTimer = 0.0f;
			FallVolume += 10;
			Mix_Volume(4, FallVolume);
		}
		else {
			FallVolumeTimer += deltaTime;
		}
		//std::cout << "FALL" << std::endl;
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::SLIDING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.SlideAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
			mAnimationHandler.SlideAnimation.AnimationTextureIndexes[0], mAnimationHandler.SlideAnimation.Size, mAnimationHandler.SlideAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		//std::cout << "SLIDE" << std::endl;
		if (!Mix_Playing(5)) {
			Mix_PlayChannel(5, mAudioHandler.Slide, 0);
		}
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::SLAMMING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		//mActor.mSprite.vertexData.Size = mAnimationHandler.SlamAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.SlamAnimation.Size * mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.SlamAnimation.AnimationTextureIndexes[0], mAnimationHandler.SlamAnimation.Size, mAnimationHandler.SlamAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		//std::cout << "SLAM" << std::endl;
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::WALLSLIDING:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		//mActor.mSprite.vertexData.Size = mAnimationHandler.WallSlideAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mAnimationHandler.WallSlideAnimation.Size * mActor.mSizeMultiplier, mCamera.GetProjectionMatrix(),
			mAnimationHandler.WallSlideAnimation.AnimationTextureIndexes[0], mAnimationHandler.WallSlideAnimation.Size, mAnimationHandler.WallSlideAnimation.TexturePosition, mActor.mModelMatrix, !flipped);
		//std::cout << "WALL" << std::endl;
		if (!Mix_Playing(6)) {
			Mix_PlayChannel(6, mAudioHandler.WallSlide, 0);
		}
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	case PlayerStates::DEAD:

		break;
	case PlayerStates::HIT:

		break;
	case PlayerStates::IDLE:
		mActor.mSprite.vertexData.Size = mAnimationHandler.FallAnimation.Size * mActor.mSizeMultiplier;
		mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, mCamera.GetProjectionMatrix(),
			mAnimationHandler.IdleAnimation.AnimationTextureIndexes[0], mAnimationHandler.IdleAnimation.Size, mAnimationHandler.IdleAnimation.TexturePosition, mActor.mModelMatrix, flipped);
		//std::cout << "IDLE" << std::endl;
		mAnimationHandler.RunAnimation.index = 0;
		runAnimOneShot = true;
		break;
	}
	
	if (mMovementHandler.CheckPlayerStateChange()) {
		if (mMovementHandler.CompareToLastState(PlayerStates::SLAMMING) && mMovementHandler.currentPlayerState != PlayerStates::SLAMMING) {
			mAudioHandler.PlayNextLandHardSound();
		}

		if (mMovementHandler.CompareToLastState(PlayerStates::FALLING) && mMovementHandler.currentPlayerState != PlayerStates::FALLING && mMovementHandler.currentPlayerState != PlayerStates::SLAMMING && mMovementHandler.currentPlayerState != PlayerStates::DOUBLE_JUMPING) {
			mAudioHandler.PlayNextLandSoftSound();
		}

		if (mMovementHandler.currentPlayerState != PlayerStates::SLIDING) {
			Mix_HaltChannel(5);
		}
		if (mMovementHandler.currentPlayerState != PlayerStates::WALLSLIDING) {
			Mix_HaltChannel(6);
		}
		if (mMovementHandler.currentPlayerState != PlayerStates::FALLING) {
			Mix_HaltChannel(4);
			Mix_Volume(4, 0);
			FallVolume = 1;
		}

	}


	std::cout << glm::to_string(mActor.velocity) << std::endl;

	//uint32_t test123 = 529;
	//glm::vec2 test5 = mAnimationHandler.FallAnimation.Size;
	//glm::vec2 test6 = mAnimationHandler.FallAnimation.TexturePosition;
	//
	//
	//mBatchRenderer.DrawSeperatly(mActor.mSprite.vertexData.Position, mActor.mSprite.vertexData.Size, mCamera.GetProjectionMatrix(), test123, test5, test6, mActor.mModelMatrix);
	//std::cout << glm::to_string(mActor.velocity) << std::endl;

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
