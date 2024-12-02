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

void MovementHandler::Jump(float& deltaTime, const float& jumpSpeed) {
	app().mActor.velocity.y += jumpSpeed * deltaTime;
}

void MovementHandler::Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit) {
	app().mActor.velocity.y -= slamSpeed * deltaTime;

	if (app().mActor.velocity.y < -speedLimit) {
		app().mActor.velocity.y = -speedLimit;
	}

}
void MovementHandler::Update(float& deltaTime) {

	CheckPlayerState();
	if (!debugMove) {


		// Gravity and wall stick vvv
		if (canWallStick) {
			//if (app().mActor.velocity.y >= -100.0f) {
			//	app().mActor.velocity.y = -100.0f;
			//}
			//app().mActor.velocity.y -= 640.0f * deltaTime;
			app().mActor.velocity.y = -125.0f;
			//125
		}


		if (app().mActor.velocity.y <= 0.0f) {
			//if ((app().mActor.isWallMountableR && app().mMovementHandler.RightWallHug) && (app().mActor.velocity.y < 0.0f || KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)])) {
			//else if ((app().mActor.isWallMountableL && app().mMovementHandler.LeftWallHug) && (app().mActor.velocity.y < 0.0f || KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)])) {
			//if(canWallStick) {
			//	if (app().mActor.velocity.y >= -100.0f) {
			//		app().mActor.velocity.y = -100.0f;
			//	}
			//	app().mActor.velocity.y -= 640.0f * deltaTime;
			//}
			//else {
			//}
			app().mActor.velocity.y -= 1920.0f * deltaTime;
		}
		else if (app().mActor.velocity.y > 0.0f) {
			app().mActor.velocity.y -= 1440.0f * deltaTime;
		}
		// Gravity and wall stick ^^^

		// Side movement vvv
		if (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)]) {
			if (isGrounded) {
				if (!(app().mActor.velocity.x + -4000.0f * deltaTime < -500.0f)) {
					Move(app().mActor.velocity, glm::vec2(-4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(app().mActor.velocity.x + -1440.0f * deltaTime < -500.0f)) {
					Move(app().mActor.velocity, glm::vec2(-1440.0f, 0.0f) * deltaTime);
				}
			}
			if (AOneShot) {
				//std::cout << "AOneShot" << std::endl;
				AOneShot = false;
			}
			lookDirection = LookDirections::LEFT;
		}
		if (KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)]) {
			if (isGrounded) {
				if (!(app().mActor.velocity.x + 4000.0f * deltaTime > 500.0f)) {
					Move(app().mActor.velocity, glm::vec2(4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(app().mActor.velocity.x + 1440.0f * deltaTime > 500.0f)) {
					Move(app().mActor.velocity, glm::vec2(1440.0f, 0.0f) * deltaTime);
				}
			}
			if (DOneShot) {
				//std::cout << "DOneShot" << std::endl;
				DOneShot = false;
			}
			lookDirection = LookDirections::RIGHT;
		}
		// Side movement ^^^

		if (Sign(app().mActor.velocity.x) == -1) {
			lookDirection = LookDirections::LEFT;
		}
		else if (Sign(app().mActor.velocity.x) == 1) {
			lookDirection = LookDirections::RIGHT;
		}


		// Slam and slide vvv
		if (KeyboadStates[static_cast<int>(MovementState::DUCK)]) {

			if (duckOneShot) {
				if (!isGrounded) { 
					isSlamming = true;
					canDoubleJump = false;
					duckOneShot = false;
				}
				if (isGrounded && (app().mActor.velocity.x > 200.0f || app().mActor.velocity.x < -200.0f)) {
					slideDirection = Sign(app().mActor.velocity.x);
					isSliding = true;
					duckOneShot = false;
				}
			}
			
		}


		if (!KeyboadStates[static_cast<int>(MovementState::DUCK)] || !isGrounded) {
			slideOneShot = true;
		}

		if (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && slideDirection == 1) {
			isSliding = false;
		}

		if (KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && slideDirection == -1) {
			isSliding = false;
		}


		if (!KeyboadStates[static_cast<int>(MovementState::DUCK)]) {
			frictionModifier = 7.2f;
			isSliding = false; 
		}


		if (isSlamming) {
			Slam(deltaTime, 5000.0f, 2000.0f);
		}

		if (isSliding) {
			if (slideOneShot && isGrounded) {
				frictionModifier = 0.25f;
				app().mActor.velocity.x = app().mActor.velocity.x + 200.0f * slideDirection;
				slideOneShot = false;
			}
		}
		

		if (isGrounded || (!isGrounded && canWallStick)) {
			isSlamming = false;
		}


		// Slam and slide ^^^

		// Jumping, wall jumping, double jumping vvv
		if (KeyboadStates[static_cast<int>(MovementState::SPACE)]) {
			if (spacebarOneShot)
			{
				if (canWallStick) {
					isWallJumping = true;
				}
				
				jumpBufferTimer = std::chrono::high_resolution_clock::now();
				
				if (!isGrounded && !canWallStick && canDoubleJump) {
					canDoubleJump = false;
					app().mActor.velocity.y = 320.0f;
					doubleJumpTimer = std::chrono::high_resolution_clock::now();
				}
			}
			spacebarOneShot = false;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - doubleJumpTimer).count() < doubleJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 3000.0f);
			}

			if (isJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer).count() < jumpTime + deltaTime * 1000) {
				Jump(deltaTime, 3000.0f);
			}

			if (isWallJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - wallJumpTimer).count() < wallJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 2000.0f);
			}
		}
		if (!KeyboadStates[static_cast<int>(MovementState::SPACE)]) { 
			isJumping = false;
			isWallJumping = false;
		}
		
		if (canWallStick && isWallJumping) {
			canDoubleJump = true;
			wallJumpTimer = std::chrono::high_resolution_clock::now();
			app().mActor.velocity.y = 320.0f;
			if (app().mActor.isWallMountableL) {
				app().mActor.velocity.x = 300.0f;
			}
			else if (app().mActor.isWallMountableR) {
				app().mActor.velocity.x = -300.0f;
			}
			app().mActor.isWallMountableL = false;
			app().mActor.isWallMountableR = false;
		}

		if (((app().mActor.isWallMountableR && RightWallHug) || (app().mActor.isWallMountableL && LeftWallHug)) && !(KeyboadStates[static_cast<int>(MovementState::SPACE)] && app().mActor.velocity.y > 0.0f)) {
			canWallStick = true;
			canDoubleJump = true;
		}
		else {
			canWallStick = false;
		}

		if (isGrounded 
				&& std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < jumpBufferTime + deltaTime * 1000) {
			//Jump();
			canDoubleJump = true;
			isJumping = true;
			jumpTimer = std::chrono::high_resolution_clock::now();
			app().mActor.velocity.y += 320.0f;
			isGrounded = false;
			//jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1, 1000000000>>>::min();
		}

		if (isGrounded) {
			canDoubleJump = true;
			isJumping = false;
		}

		if ((app().mActor.isWallMountableR && RightWallHug) || (app().mActor.isWallMountableL && LeftWallHug)) {
			isWallJumping = false;
		}

		// Jumping, wall jumping, double jumping ^^^

		// Friction and limits vvvq

		if (isGrounded) {
			float nextActorVelocityX = app().mActor.velocity.x *= 1.0f - (frictionModifier * deltaTime);
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
		if (app().mActor.velocity.x > 800.0f) {
			app().mActor.velocity.x = 800.0f;
		}
		if (app().mActor.velocity.x < -800.0f) {
			app().mActor.velocity.x = -800.0f;
		}
		if (app().mActor.velocity.y < -1200.0f) {
			app().mActor.velocity.y = -1200.0f;
		}

		// Friction and limits ^^^

		//std::cout << glm::to_string(app().mActor.velocity) << std::endl;
		//app().mActor.velocity = glm::vec2(float(std::round(app().mActor.velocity.x * 10000)) / 10000.0f, float(std::round(app().mActor.velocity.y * 10000)) / 10000.0f);
	}
	else {
	}
} 

void MovementHandler::CheckPlayerState() {
	lastState = currentPlayerState;
	if (!isGrounded && !canWallStick && canDoubleJump) {
		if (app().mActor.velocity.y > 0.0f) {
			app().temp_color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::JUMPING;
			// jump
		}
		else if (app().mActor.velocity.y <= 0.0f) {
			app().temp_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::FALLING;
			// fall
		}
	}
	else if (!isGrounded && !canWallStick && !canDoubleJump) {
		// double jump
		if (app().mActor.velocity.y > 0.0f) {
			app().temp_color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::DOUBLE_JUMPING;
			// jump
		}
		else if (app().mActor.velocity.y <= 0.0f) {
			app().temp_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::FALLING;
			// fall
		}
	}
	else if (!isGrounded && canWallStick) {
		app().temp_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		currentPlayerState = PlayerStates::WALLSLIDING;
		// wall slide
	}
	else {
		if (!(app().mActor.velocity.x < 1.0f && app().mActor.velocity.x > -1.0f) && (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] || KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)])) {
			app().temp_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::RUNNING;
			// run
		}
		else {
			app().temp_color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
			currentPlayerState = PlayerStates::IDLE;
			// idle
		}
	}
	if (isSliding && isGrounded) {
		app().temp_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		currentPlayerState = PlayerStates::SLIDING; 
		// slide
	}
	if (isSlamming) {
		app().temp_color = glm::vec4(0.5f, 0.0f, 1.0f, 1.0f);
		currentPlayerState = PlayerStates::SLAMMING;
		// slam
	}
}

bool MovementHandler::CheckPlayerStateChange() {
	return currentPlayerState != lastState;
}

bool MovementHandler::CompareToLastState(const PlayerStates& state) {
	return state == lastState;
}