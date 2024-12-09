#include "MovementHandler.hpp"
#include "glm/gtx/string_cast.hpp"

MovementHandler::MovementHandler() {
}

MovementHandler::~MovementHandler() {
}

void MovementHandler::Move(glm::vec2& actorVelocity, const glm::vec2& acceleration) {
	actorVelocity = glm::vec2(actorVelocity.x + acceleration.x, actorVelocity.y + acceleration.y);
}

void MovementHandler::Jump(float& deltaTime, const float& jumpSpeed, glm::vec2& actorVelocity) {
	actorVelocity.y += jumpSpeed * deltaTime;
}

void MovementHandler::Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit, glm::vec2& actorVelocity) {
	actorVelocity.y -= slamSpeed * deltaTime;

	if (actorVelocity.y < -speedLimit) {
		actorVelocity.y = -speedLimit;
	}

}
void MovementHandler::Update(float& deltaTime, Actor& actor) {
	if ((!actor.isSucked && !actor.isConsumedByVoid && !actor.mEscaped && !actor.isSuckedPortal)) {

		// Gravity and wall stick vvv
		if (canWallStick) {
			actor.velocity.y = -125.0f;
		}

		if (actor.velocity.y <= 0.0f) {
			actor.velocity.y -= 1920.0f * deltaTime;
		}
		else if (actor.velocity.y > 0.0f) {
			actor.velocity.y -= 1440.0f * deltaTime;
		}
		// Gravity and wall stick ^^^

		// Side movement vvv
		if (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && !actor.mDead) {
			if (isGrounded) {
				if (!(actor.velocity.x + -4000.0f * deltaTime < -500.0f)) {
					Move(actor.velocity, glm::vec2(-4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.velocity.x + -1440.0f * deltaTime < -500.0f)) {
					Move(actor.velocity, glm::vec2(-1440.0f, 0.0f) * deltaTime);
				}
			}
			lookDirection = LookDirections::LEFT;
		}
		if (KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && !actor.mDead) {
			if (isGrounded) {
				if (!(actor.velocity.x + 4000.0f * deltaTime > 500.0f)) {
					Move(actor.velocity, glm::vec2(4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.velocity.x + 1440.0f * deltaTime > 500.0f)) {
					Move(actor.velocity, glm::vec2(1440.0f, 0.0f) * deltaTime);
				}
			}
			lookDirection = LookDirections::RIGHT;
		}
		// Side movement ^^^

		if (Sign(actor.velocity.x) == -1) {
			lookDirection = LookDirections::LEFT;
		}
		else if (Sign(actor.velocity.x) == 1) {
			lookDirection = LookDirections::RIGHT;
		}


		// Slam and slide vvv
		if (KeyboadStates[static_cast<int>(MovementState::DUCK)] && !actor.mDead) {

			if (duckOneShot) {
				if (!isGrounded) { 
					isSlamming = true;
					canDoubleJump = false;
					duckOneShot = false;
				}
				if (isGrounded && (actor.velocity.x > 200.0f || actor.velocity.x < -200.0f)) {
					slideDirection = Sign(actor.velocity.x);
					isSliding = true;
					duckOneShot = false;
				}
			}
			
		}


		if (!KeyboadStates[static_cast<int>(MovementState::DUCK)] || !isGrounded) {
			slideOneShot = true;
		}

		if (KeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && slideDirection == 1 && !actor.mDead) {
			isSliding = false;
		}

		if (KeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && slideDirection == -1 && !actor.mDead) {
			isSliding = false;
		}


		if (!KeyboadStates[static_cast<int>(MovementState::DUCK)]) {
			frictionModifier = 7.2f;
			isSliding = false; 
		}


		if (isSlamming) {
			Slam(deltaTime, 5000.0f, 2000.0f, actor.velocity);
		}

		if (isSliding) {
			if (slideOneShot && isGrounded) {
				frictionModifier = 0.25f;
				actor.velocity.x = actor.velocity.x + 200.0f * slideDirection;
				slideOneShot = false;
			}
		}
		

		if (isGrounded || (!isGrounded && canWallStick)) {
			isSlamming = false;
		}


		// Slam and slide ^^^

		// Jumping, wall jumping, double jumping vvv
		if (KeyboadStates[static_cast<int>(MovementState::SPACE)] && !actor.mDead) {
			if (spacebarOneShot)
			{
				if (canWallStick) {
					isWallJumping = true;
				}
				
				jumpBufferTimer = std::chrono::high_resolution_clock::now();
				
				if (!isGrounded && !canWallStick && canDoubleJump) {
					canDoubleJump = false;
					actor.velocity.y = 320.0f;
					doubleJumpTimer = std::chrono::high_resolution_clock::now();
				}
			}
			spacebarOneShot = false;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - doubleJumpTimer).count() < doubleJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 3000.0f, actor.velocity);
			}

			if ((isJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer).count() < jumpTime + deltaTime * 1000)) {
				Jump(deltaTime, 3000.0f, actor.velocity);
			}

			if (isWallJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - wallJumpTimer).count() < wallJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 2000.0f, actor.velocity);
			}
		}
		if (!KeyboadStates[static_cast<int>(MovementState::SPACE)]) { 
			isJumping = false;
			isWallJumping = false;
		}
		
		if (canWallStick && isWallJumping) {
			canDoubleJump = true;
			wallJumpTimer = std::chrono::high_resolution_clock::now();
			actor.velocity.y = 320.0f;
			if (actor.isWallMountableL) {
				actor.velocity.x = 300.0f;
			}
			else if (actor.isWallMountableR) {
				actor.velocity.x = -300.0f;
			}
			actor.isWallMountableL = false;
			actor.isWallMountableR = false;
		}

		if (((actor.isWallMountableR && RightWallHug) || (actor.isWallMountableL && LeftWallHug)) && !(KeyboadStates[static_cast<int>(MovementState::SPACE)] && actor.velocity.y > 0.0f)) {
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
			actor.velocity.y += 320.0f;
			isGrounded = false;
		}

		if (isGrounded) {
			canDoubleJump = true;
			isJumping = false;
		}

		if ((actor.isWallMountableR && RightWallHug) || (actor.isWallMountableL && LeftWallHug)) {
			isWallJumping = false;
		}

		// Jumping, wall jumping, double jumping ^^^

		// Friction and limits vvvq

		if (isGrounded) {
			float nextActorVelocityX = actor.velocity.x *= 1.0f - (frictionModifier * deltaTime);
			if (nextActorVelocityX > 0.1f || nextActorVelocityX < -0.1f) {
				actor.velocity.x = nextActorVelocityX;
			}
			else {
				actor.velocity.x = 0.0f;
			}
		}
		if (actor.velocity.x > 800.0f) {
			actor.velocity.x = 800.0f;
		}
		if (actor.velocity.x < -800.0f) {
			actor.velocity.x = -800.0f;
		}
		if (actor.velocity.y < -1200.0f) {
			actor.velocity.y = -1200.0f;
		}

		// Friction and limits ^^^
	}
} 