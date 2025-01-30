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

	void SetProjectionMatrix(const float width);

	glm::vec2 mCameraOffset{ 0.0f, 0.0f };

	glm::mat4 mUIModelMatrix{1.0f};

	glm::vec2 mCameraPosition{ 0.0f, 0.0f };

private:
	glm::mat4 projectionMatrix;
	glm::mat4 initialProjectionMatrix;

	float cameraOffsetTimerBuffer{ 0.0f };
	float cameraOffsetTimeBuffer{ 0.5f };
	float cameraOffsetTimerBuffer2{ 0.0f };
	float cameraOffsetTimeBuffer2{ 0.5f };
};

