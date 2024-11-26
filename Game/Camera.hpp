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

	glm::mat4 GetViewMatrix() const;

	glm::mat4 GetProjectionMatrix() const;

	void SetProjectionMatrix();

	void SetMousePosstition(double mouseX, double mouseY);
	glm::vec2 mCameraVelocity{ 0.0f, 0.0f };

	glm::vec2 GetMousePossition() const;
	glm::vec3 mEye;

	glm::mat4 mUIModelMatrix{1.0f};

private:
	glm::vec2 mMousePossition;

	glm::mat4 mProjectionMatrix;
	glm::mat4 mInitialProjectionMatrix;

	glm::vec3 mViewDirection;
	glm::vec3 mUpVector;

	glm::vec2 mOldMousePosition;

	float mCameraVelocityTimerBuffer{ 0.0f };
	float mCameraVelocityTimeBuffer{ 0.5f };
	float mCameraVelocityTimerBuffer2{ 0.0f };
	float mCameraVelocityTimeBuffer2{ 0.5f };
};

