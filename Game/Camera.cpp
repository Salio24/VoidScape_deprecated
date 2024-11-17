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
	mEye = glm::vec3(0.0f, 0.0f, 0.0f);
	mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

App& Camera::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

void Camera::Update(glm::vec2& actorVelocity, glm::vec2& actorScreenPosition, const float& deltaTime) {
	if (!(actorScreenPosition.x > -0.2f && actorScreenPosition.x < 0.2f)) {
		//mProjectionMatrix = glm::translate(mProjectionMatrix, glm::vec3(-app().mActor.velocity.x * app().deltaTime, 0.0f, 0.0f));
		//mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-relativeActorPosition.x, 0.0f, 0.0f));
	}

	if (app().mActor.mPosition.x > 0.0f) {
		//if (actorScreenPosition.x > 0.2f && Sign(actorVelocity.x) == 1) {
		//	mProjectionMatrix = glm::translate(mProjectionMatrix, glm::vec3(-actorVelocity.x * deltaTime, 0.0f, 0.0f));
		//	mUIModelMatrix = glm::translate(mUIModelMatrix, glm::vec3(actorVelocity.x * deltaTime, 0.0f, 0.0f));
		//}
		//if (actorScreenPosition.x < -0.2f && Sign(actorVelocity.x) == -1) {
		//	mProjectionMatrix = glm::translate(mProjectionMatrix, glm::vec3(-actorVelocity.x * deltaTime, 0.0f, 0.0f));
		//	mUIModelMatrix = glm::translate(mUIModelMatrix, glm::vec3(actorVelocity.x * deltaTime, 0.0f, 0.0f));
		//}
		if (int(actorVelocity.x) > 0.0f && mCameraVelocityTimerBuffer2 <= 0.f) {
			mCameraVelocity.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraVelocity.x > 350.0f) {
				mCameraVelocity.x = 350.0f;
			}
			mCameraVelocityTimerBuffer = mCameraVelocityTimeBuffer;
			//mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 960.0f + mCameraVelocity.x), 0.0f, 0.0f));
		}
		else if (int(actorVelocity.x) < 0.0f && mCameraVelocityTimerBuffer <= 0.f) {
			mCameraVelocity.x += (actorVelocity.x * deltaTime) / 5.0f;
			if (mCameraVelocity.x < -350.0f) {
				mCameraVelocity.x = -350.0f;
			}
			mCameraVelocityTimerBuffer2 = mCameraVelocityTimeBuffer2;
			//mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 960.0f + mCameraVelocity.x), 0.0f, 0.0f));
		}
			//mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 960.0f + mCameraVelocity.x), 0.0f, 0.0f));

		if (mCameraVelocityTimerBuffer > 0.0f) {
			mCameraVelocityTimerBuffer -= deltaTime;
		}
		if (mCameraVelocityTimerBuffer2 > 0.0f) {
			mCameraVelocityTimerBuffer2 -= deltaTime;
		}

		
		else {
			if (mCameraVelocity.x > 0.0f && mCameraVelocityTimerBuffer <= 0.0f && mCameraVelocityTimerBuffer2 <= 0.0f) {
				if (mCameraVelocity.x > 0.25f) {
					mCameraVelocity.x -= 36.0f * deltaTime;
				}
				else {
					mCameraVelocity.x = 0;
				}
			}
			else if (mCameraVelocity.x < 0.0f && mCameraVelocityTimerBuffer <= 0.0f && mCameraVelocityTimerBuffer2 <= 0.0f) {
				if (mCameraVelocity.x < -0.25f) {
					mCameraVelocity.x += 36.0f * deltaTime;
				}
				else {
					mCameraVelocity.x = 0;
				}
			}
			//mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 960.0f), 0.0f, 0.0f));
		}
		mProjectionMatrix = glm::translate(mInitialProjectionMatrix, glm::vec3(-(app().mActor.mPosition.x - 800.0f + mCameraVelocity.x), 0.0f, 0.0f));
		mUIModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((app().mActor.mPosition.x - 800.0f + mCameraVelocity.x), 0.0f, 0.0f));
		//std::cout << mCameraVelocity.x << ", " << actorVelocity.x << std::endl;
	}
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
}

void Camera::SetProjectionMatrix() {
	mInitialProjectionMatrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
	mProjectionMatrix = mInitialProjectionMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
	return mProjectionMatrix;
}
void Camera::SetMousePosstition(double mouseX, double mouseY) {
	mMousePossition = glm::vec2(mouseX / (float)app().mWindowWidth * 320.0f,mouseY / (float)app().mWindowHeight * 180.0f);
}

glm::vec2 Camera::GetMousePossition() const {
	return mMousePossition;
}




