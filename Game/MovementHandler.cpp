#include "MovementHandler.hpp"
#include "App.hpp"
#include "glm/gtx/string_cast.hpp"

MovementHandler::MovementHandler() : app_(nullptr) {
}

MovementHandler::~MovementHandler() {
}

App& MovementHandler::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

void MovementHandler::Move(glm::vec2& actorVelocity, const glm::vec2& acceleration) {
	actorVelocity = glm::vec2(actorVelocity.x + acceleration.x, actorVelocity.y + acceleration.y);
	std::cout << actorVelocity.x << std::endl;
	//TODO: add appropriate delta time
}

void MovementHandler::WallJump() {
	if (app().mActor.isWallMountableL) {
		app().mActor.velocity.y = 500.0f;
		app().mActor.velocity.x = 300.0f;
	}
	else if (app().mActor.isWallMountableR) {
		app().mActor.velocity.y = 500.0f;
		app().mActor.velocity.x = -300.0f;
	}
	app().mActor.isWallMountableL = false;
	app().mActor.isWallMountableR = false;
}

void MovementHandler::Jump(float& deltaTime, float& foo) {
	app().mActor.velocity.y += 2000.0f * deltaTime;
	foo += 2000.0f * deltaTime;
}

void MovementHandler::Update(float& deltaTime) {
	if (!debugMove) {

		if (app().mActor.velocity.y <= 0.0f) {
			if ((app().mActor.isWallMountableR && app().mMovementHandler.RightWallHug) && (app().mActor.velocity.y < 0.0f || KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)])) {
				if (app().mActor.velocity.y >= -100.0f) {
					app().mActor.velocity.y = -100.0f;
				}
				app().mActor.velocity.y -= 640.0f * deltaTime;
			}
			else if ((app().mActor.isWallMountableL && app().mMovementHandler.LeftWallHug) && (app().mActor.velocity.y < 0.0f || KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)])) {
				if (app().mActor.velocity.y >= -100.0f) {
					app().mActor.velocity.y = -100.0f;
				}
				app().mActor.velocity.y -= 640.0f * deltaTime;
			}
			else {
				app().mActor.velocity.y -= 1920.0f * deltaTime;
			}
			gstate = true;
		}
		else if (app().mActor.velocity.y > 0.0f) {
			gstate = false;
			app().mActor.velocity.y -= 1440.0f * deltaTime;
		}

		if (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)]) {
			if (app().mActor.isGrounded) {
				Move(app().mActor.velocity, glm::vec2(-4000.0f * deltaTime, 0.0f));
			}
			else {
				Move(app().mActor.velocity, glm::vec2(-1440.0f * deltaTime, 0.0f));
			}
		}
		if (KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)]) {
			if (app().mActor.isGrounded) {
				Move(app().mActor.velocity, glm::vec2(4000.0f, 0.0f) * deltaTime);
			}
			else {
				Move(app().mActor.velocity, glm::vec2(1440.0f, 0.0f) * deltaTime);
			}
		}

		if (KeyboadStates[static_cast<int>(MovementState::SPACE)]) {
			if (spacebarOneShot)
			{
				if ((app().mActor.isWallMountableR && RightWallHug) || (app().mActor.isWallMountableL && LeftWallHug)) {
					WallJump();
				}
				foo = 0;
				jumpBufferTimer = std::chrono::high_resolution_clock::now();

			}
			spacebarOneShot = false;
			if (isJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer).count() < jumpTime + deltaTime * 100) {
				Jump(deltaTime, foo);
				//std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer2).count() << "         ";
			}
		}
		//if (!KeyboadStates[static_cast<int>(MovementState::SPACE)] || std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - jumpTimer2).count() >= jumpTime) {
		//	isJumping = false;
		//}

		if (app().mActor.isGrounded && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < jumpBufferTime + deltaTime * 100) {
			//Jump();
			isJumping = true;
			jumpTimer = std::chrono::high_resolution_clock::now();
			app().mActor.velocity.y += 320.0f;
			app().mActor.isGrounded = false;
			//jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1, 1000000000>>>::min();
		}

		if (app().mActor.isGrounded) {
			isJumping = false;
		}

		if (app().mActor.isGrounded) {
			//jumpBufferTimer = 0.0f;
		}

		//std::cout << isJumping << ", " << app().mActor.isGrounded << ", " << gstate << ", " << foo << ", " << deltaTime << std::endl;
		if (app().mActor.isGrounded) {
			float nextActorVelocityX = app().mActor.velocity.x *= 1.0f - (7.2f * deltaTime);
			if (nextActorVelocityX > 0.1f || nextActorVelocityX < -0.1f) {
				app().mActor.velocity.x = nextActorVelocityX;
			}
			else {
				app().mActor.velocity.x = 0.0f;
			}
		}
		//if (app().mActor.velocity.x < 0.001f && app().mActor.velocity.x > 0.0f) {
		//	app().mActor.velocity.x = 0.0f;
		//}
		//else if (app().mActor.velocity.x > -0.001f && app().mActor.velocity.x < 0.0f) {
		//	app().mActor.velocity.x = 0.0f;
		//}
		if (app().mActor.velocity.x > 500.0f) {
			app().mActor.velocity.x = 500.0f;
		}
		if (app().mActor.velocity.x < -500.0f) {
			app().mActor.velocity.x = -500.0f;
		}
		std::cout << glm::to_string(app().mActor.velocity) << std::endl;
		//app().mActor.velocity = glm::vec2(float(std::round(app().mActor.velocity.x * 10000)) / 10000.0f, float(std::round(app().mActor.velocity.y * 10000)) / 10000.0f);
	}
	else {
	}
} 