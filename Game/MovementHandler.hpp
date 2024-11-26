#pragma once
#include <glm/glm.hpp>
#include "HelperStructs.hpp"
#include <chrono>
#include "Sign.hpp"

class App;

class MovementHandler {

	App& app();

	App* app_;

	glm::vec2 acceleration{ 0.0f, 0.0f };

	void Move(glm::vec2& actorVelocity, const glm::vec2& acceleration);

	void WallJump();

	void Jump(float& deltaTime, const float& jumpSpeed);

	void Slam(float& deltaTime, const float& slamSpeed, const float& speedLimit);

	bool debugMove{ false };

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
	MovementHandler();

	~MovementHandler();

	bool canDoubleJump{ false };

	bool isSliding{ false };
	bool canWallStick{ false };

	void Update(float& deltaTime);

	bool spacebarOneShot{ true };

	bool duckOneShot{ true };

	bool KeyboadStates[static_cast<int>(MovementState::END)] = { false };

	bool LeftWallHug{ false };
	bool RightWallHug{ false };

	bool GravityState{ true };

};


