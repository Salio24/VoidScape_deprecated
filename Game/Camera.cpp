#include "Camera.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/vec3.hpp>
#include <algorithm>
#include "glm/gtx/string_cast.hpp"
#include "App.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Sign.hpp"


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
		if (int(actorVelocity.x) > 0.0f && mCameraOffsetTimerBuffer2 <= 0.f) {
			mCameraOffset.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraOffset.x > 350.0f) {
				mCameraOffset.x = 350.0f;
			}
			mCameraOffsetTimerBuffer = mCameraOffsetTimeBuffer;
		}
		else if (int(actorVelocity.x) < 0.0f && mCameraOffsetTimerBuffer <= 0.f) {
			mCameraOffset.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraOffset.x < -350.0f) {
				mCameraOffset.x = -350.0f;
			}
			mCameraOffsetTimerBuffer2 = mCameraOffsetTimeBuffer2;
		}

		if (mCameraOffsetTimerBuffer > 0.0f) {
			mCameraOffsetTimerBuffer -= deltaTime;
		}
		if (mCameraOffsetTimerBuffer2 > 0.0f) {
			mCameraOffsetTimerBuffer2 -= deltaTime;
		}

		
		else {
			if (mCameraOffset.x > 0.0f && mCameraOffsetTimerBuffer <= 0.0f && mCameraOffsetTimerBuffer2 <= 0.0f) {
				if (mCameraOffset.x > 0.25f) {
					mCameraOffset.x -= 36.0f * deltaTime;
				}
				else {
					mCameraOffset.x = 0;
				}
			}
			else if (mCameraOffset.x < 0.0f && mCameraOffsetTimerBuffer <= 0.0f && mCameraOffsetTimerBuffer2 <= 0.0f) {
				if (mCameraOffset.x < -0.25f) {
					mCameraOffset.x += 36.0f * deltaTime;
				}
				else {
					mCameraOffset.x = 0;
				}
			}
		}
		mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 800.0f + mCameraOffset.x), 0.0f, 0.0f));
		mUIModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((app().mActor.mPosition.x - 800.0f + mCameraOffset.x), 0.0f, 0.0f));
	}
}

void Camera::SetProjectionMatrix() {
	mInitialProjectionMatrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
	mProjectionMatrix = mInitialProjectionMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
	return mProjectionMatrix;
}




