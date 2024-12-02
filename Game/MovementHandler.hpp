#pragma once
#include <glm/glm.hpp>
#include <chrono>
#include "Sign.hpp"

class App;

enum class MovementState {
	MOVE_LEFT,
	MOVE_RIGHT,
	SPACE,
	DUCK,
	MOVE_UP,
	MOVE_DOWN,
	END
};

enum class PlayerStates {
	RUNNING,
	JUMPING,
	DOUBLE_JUMPING,
	FALLING,
	SLIDING,
	SLAMMING,
	WALLSLIDING,
	DEAD,
	HIT,
	IDLE,
	END
};

enum class LookDirections {
	LEFT,
	RIGHT,
	END
};

class MovementHandler {

	App& app();

	App* app_;

	glm::vec2 acceleration{ 0.0f, 0.0f };

	void Move(glm::vec2& actorVelocity, const glm::vec2& acceleration);

	void WallJump();

	void Jump(float& deltaTime, const float& jumpSpeed);

	void Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit);

	void CheckPlayerState();

	float frictionModifier{ 7.2f };

	const int jumpBufferTime{ 100 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpBufferTimer;

	//const int wallJumpBufferTime{ 100 };
	//std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> wallJumpBufferTimer; 

	const int jumpTime{ 200 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpTimer;

	const int wallJumpTime{ 200 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> wallJumpTimer;

	const int doubleJumpTime{ 150 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> doubleJumpTimer;

	bool isJumping{ false };

	bool isWallJumping{ false }; 

	bool isDoubleJumping{ false };

	bool isSlamming{ false };


	bool slideOneShot{ true };

	int slideDirection{ 0 };


public:
	PlayerStates lastState = PlayerStates::IDLE;
	MovementHandler();

	~MovementHandler();

	bool CheckPlayerStateChange();

	bool CompareToLastState(const PlayerStates& state);

	bool canDoubleJump{ false };
	bool isGrounded{ false };

	bool debugMove{ false };

	bool isSliding{ false };
	bool canWallStick{ false };

	void Update(float& deltaTime);

	bool spacebarOneShot{ true };

	bool duckOneShot{ true };

	bool AOneShot{ true };

	bool DOneShot{ true };

	bool KeyboadStates[static_cast<int>(MovementState::END)] = { false };

	PlayerStates currentPlayerState = PlayerStates::IDLE; // Initialize with a default state

	LookDirections lookDirection = LookDirections::RIGHT;

	bool LeftWallHug{ false };
	bool RightWallHug{ false };

	bool GravityState{ true };

};


