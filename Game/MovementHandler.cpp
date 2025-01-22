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

void MovementHandler::FinishDoubleJump() {
	doubleJumpTimer = std::chrono::time_point<std::chrono::steady_clock>{};
}

void MovementHandler::FinishJump() {
	jumpTimer = std::chrono::time_point<std::chrono::steady_clock>{};
}

void MovementHandler::Update(float& deltaTime, Actor& actor, const bool& started) {
	if ((!actor.mIsSucked && !actor.mConsumedByBlackHole && !actor.mEscaped && !actor.mIsSuckedPortal) && started) {

		mLast_mCanDoubleJump = mCanDoubleJump;
		// Gravity and wall stick vvv
		if (mCanWallStick) {
			actor.mVelocity.y = -125.0f;
		}

		if (actor.mVelocity.y <= 0.0f) {
			actor.mVelocity.y -= 1920.0f * deltaTime;
		}
		else if (actor.mVelocity.y > 0.0f) {
			actor.mVelocity.y -= 1440.0f * deltaTime;
		}
		// Gravity and wall stick ^^^

		// Side movement vvv
		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && !actor.mDead) {
			if (mIsGrounded) {
				if (!(actor.mVelocity.x + -4000.0f * deltaTime < -500.0f)) {
					Move(actor.mVelocity, glm::vec2(-4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.mVelocity.x + -1440.0f * deltaTime < -500.0f)) {
					Move(actor.mVelocity, glm::vec2(-1440.0f, 0.0f) * deltaTime);
				}
			}
			mLookDirection = LookDirections::LEFT;
		}
		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && !actor.mDead) {
			if (mIsGrounded) {
				if (!(actor.mVelocity.x + 4000.0f * deltaTime > 500.0f)) {
					Move(actor.mVelocity, glm::vec2(4000.0f, 0.0f) * deltaTime);
				}
			}
			else {
				if (!(actor.mVelocity.x + 1440.0f * deltaTime > 500.0f)) {
					Move(actor.mVelocity, glm::vec2(1440.0f, 0.0f) * deltaTime);
				}
			}
			mLookDirection = LookDirections::RIGHT;
		}
		// Side movement ^^^

		if (Sign(actor.mVelocity.x) == -1) {
			mLookDirection = LookDirections::LEFT;
		}
		else if (Sign(actor.mVelocity.x) == 1) {
			mLookDirection = LookDirections::RIGHT;
		}


		// Slam and slide vvv
		if (mKeyboadStates[static_cast<int>(MovementState::DUCK)] && !actor.mDead) {

			if (mDuckOneShot) {
				if (!mIsGrounded) { 
					mIsSlamming = true;
					mCanDoubleJump = false;
					mDuckOneShot = false;
				}
				if (mIsGrounded && (actor.mVelocity.x > 200.0f || actor.mVelocity.x < -200.0f)) {
					slideDirection = Sign(actor.mVelocity.x);
					mIsSliding = true;
					mDuckOneShot = false;
				}
			}
			
		}


		if (!mKeyboadStates[static_cast<int>(MovementState::DUCK)] || !mIsGrounded) {
			slideOneShot = true;
		}

		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_LEFT)] && slideDirection == 1 && !actor.mDead) {
			mIsSliding = false;
		}

		if (mKeyboadStates[static_cast<int>(MovementState::MOVE_RIGHT)] && slideDirection == -1 && !actor.mDead) {
			mIsSliding = false;
		}


		if (!mKeyboadStates[static_cast<int>(MovementState::DUCK)]) {
			frictionModifier = 7.2f;
			mIsSliding = false; 
		}


		if (mIsSlamming) {
			Slam(deltaTime, 5000.0f, 2000.0f, actor.mVelocity);
		}

		if (mIsSliding) {
			if (slideOneShot && mIsGrounded) {
				frictionModifier = 0.25f;
				actor.mVelocity.x = actor.mVelocity.x + 200.0f * slideDirection;
				slideOneShot = false;
			}
		}
		

		if (mIsGrounded || (!mIsGrounded && mCanWallStick)) {
			mIsSlamming = false;
		}


		// Slam and slide ^^^

		// Jumping, wall jumping, double jumping vvv
		if (mKeyboadStates[static_cast<int>(MovementState::SPACE)] && !actor.mDead) {
			if (mSpacebarOneShot)
			{
				if (mCanWallStick) {
					isWallJumping = true;
				}
				
				jumpBufferTimer = std::chrono::high_resolution_clock::now();
				
				if (!mIsGrounded && !mCanWallStick && mCanDoubleJump) {
					mCanDoubleJump = false;
					actor.mVelocity.y = 320.0f;
					doubleJumpTimer = std::chrono::high_resolution_clock::now();
				}
			}
			mSpacebarOneShot = false;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - doubleJumpTimer).count() < doubleJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 3000.0f, actor.mVelocity);
			}

			if ((isJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpTimer).count() < jumpTime + deltaTime * 1000)) {
				Jump(deltaTime, 3000.0f, actor.mVelocity);
			}

			if (isWallJumping && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - wallJumpTimer).count() < wallJumpTime + deltaTime * 1000) {
				Jump(deltaTime, 2000.0f, actor.mVelocity);
			}
		}
		if (!mKeyboadStates[static_cast<int>(MovementState::SPACE)]) { 
			isJumping = false;
			isWallJumping = false;
		}
		
		if (mCanWallStick && isWallJumping) {
			mCanDoubleJump = true;
			wallJumpTimer = std::chrono::high_resolution_clock::now();
			actor.mVelocity.y = 320.0f;
			if (mIsWallMountableL) {
				actor.mVelocity.x = 300.0f;
			}
			else if (mIsWallMountableR) {
				actor.mVelocity.x = -300.0f;
			}
			mIsWallMountableL = false;
			mIsWallMountableR = false;
		}

		if (((mIsWallMountableR && mRightWallHug) || (mIsWallMountableL && mLeftWallHug)) && !(mKeyboadStates[static_cast<int>(MovementState::SPACE)] && actor.mVelocity.y > 0.0f)) {
			mCanWallStick = true;
			mCanDoubleJump = true;
		}
		else {
			mCanWallStick = false;
		}

		if (mIsGrounded 
				&& std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - jumpBufferTimer).count() < jumpBufferTime + deltaTime * 1000) {
			jumpBufferTimer = std::chrono::time_point<std::chrono::steady_clock>{};
			mCanDoubleJump = true;
			isJumping = true;
			jumpTimer = std::chrono::high_resolution_clock::now();
			actor.mVelocity.y += 320.0f;
			mIsGrounded = false;
		}

		if (mIsGrounded) {
			mCanDoubleJump = true;
			isJumping = false;
		}

		if ((mIsWallMountableR && mRightWallHug) || (mIsWallMountableL && mLeftWallHug)) {
			isWallJumping = false;
		}

		// Jumping, wall jumping, double jumping ^^^

		// Friction and limits vvvq

		if (mIsGrounded) {
			float nextActorVelocityX = actor.mVelocity.x *= 1.0f - (frictionModifier * deltaTime);
			if (nextActorVelocityX > 0.1f || nextActorVelocityX < -0.1f) {
				actor.mVelocity.x = nextActorVelocityX;
			}
			else {
				actor.mVelocity.x = 0.0f;
			}
		}
		if (actor.mVelocity.x > 800.0f) {
			actor.mVelocity.x = 800.0f;
		}
		if (actor.mVelocity.x < -800.0f) {
			actor.mVelocity.x = -800.0f;
		}
		if (actor.mVelocity.y < -1200.0f) {
			actor.mVelocity.y = -1200.0f;
		}

		// Friction and limits ^^^
	}
} 