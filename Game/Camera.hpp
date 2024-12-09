#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL

class App;

class Camera {
public:

	Camera();

	App& app();

	App* app_;

	void Update(glm::vec2& actorVelocity, glm::vec2& actorScreenPosition, const float& deltaTime);

	glm::mat4 GetProjectionMatrix() const;

	void SetProjectionMatrix();

	glm::vec2 mCameraOffset{ 0.0f, 0.0f };

	glm::mat4 mUIModelMatrix{1.0f};

private:
	glm::vec2 mMousePossition;

	glm::mat4 mProjectionMatrix;
	glm::mat4 mInitialProjectionMatrix;

	glm::vec2 mOldMousePosition;

	float mCameraOffsetTimerBuffer{ 0.0f };
	float mCameraOffsetTimeBuffer{ 0.5f };
	float mCameraOffsetTimerBuffer2{ 0.0f };
	float mCameraOffsetTimeBuffer2{ 0.5f };
};

