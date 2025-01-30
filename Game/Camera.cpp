#include "Camera.hpp"
#include "App.hpp"

Camera::Camera() : app_(nullptr) {

}

App& Camera::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

void Camera::Update(glm::vec2& actorVelocity, glm::vec2& actorScreenPosition, const float& deltaTime) {
	if (app().mActor.mPosition.x > 0.0f) {
		if (int(actorVelocity.x) > 0.0f && cameraOffsetTimerBuffer2 <= 0.f) {
			mCameraOffset.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraOffset.x > 350.0f) {
				mCameraOffset.x = 350.0f;
			}
			cameraOffsetTimerBuffer = cameraOffsetTimeBuffer;
		}
		else if (int(actorVelocity.x) < 0.0f && cameraOffsetTimerBuffer <= 0.f) {
			mCameraOffset.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraOffset.x < -350.0f) {
				mCameraOffset.x = -350.0f;
			}
			cameraOffsetTimerBuffer2 = cameraOffsetTimeBuffer2;
		}

		if (cameraOffsetTimerBuffer > 0.0f) {
			cameraOffsetTimerBuffer -= deltaTime;
		}
		if (cameraOffsetTimerBuffer2 > 0.0f) {
			cameraOffsetTimerBuffer2 -= deltaTime;
		}

		
		else {
			if (mCameraOffset.x > 0.0f && cameraOffsetTimerBuffer <= 0.0f && cameraOffsetTimerBuffer2 <= 0.0f) {
				if (mCameraOffset.x > 0.25f) {
					mCameraOffset.x -= 36.0f * deltaTime;
					if (mCameraOffset.x < 0.0f) {
						mCameraOffset.x = 0.0f;
					}
				}
				else {
					mCameraOffset.x = 0;
				}
			}
			else if (mCameraOffset.x < 0.0f && cameraOffsetTimerBuffer <= 0.0f && cameraOffsetTimerBuffer2 <= 0.0f) {
				if (mCameraOffset.x < -0.25f) {
					mCameraOffset.x += 36.0f * deltaTime;
					if (mCameraOffset.x > 0.0f) {
						mCameraOffset.x = 0.0f;
					}
				}
				else {
					mCameraOffset.x = 0;
				}
			}
		}
		projectionMatrix = glm::translate(initialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 800.0f + mCameraOffset.x), 0.0f, 0.0f));
		mUIModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((app().mActor.mPosition.x - 800.0f + mCameraOffset.x), 0.0f, 0.0f));
	}

	mCameraPosition.x = app().mActor.mPosition.x - (1.0f + actorScreenPosition.x) * 960.0f + app().mActor.mSprite.mVertexData.Size.x / 2.0f;
}

void Camera::SetProjectionMatrix(const float width) {
	initialProjectionMatrix = glm::ortho(0.0f, width, 0.0f, 1080.0f, -1.0f, 1.0f);
	projectionMatrix = initialProjectionMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
	return projectionMatrix;
}




