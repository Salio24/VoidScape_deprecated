#pragma once
#include <glm/glm.hpp>
#include "HelperStructs.hpp"
#include <chrono>

class App;

class MovementHandler {

	App& app();

	App* app_;

	glm::vec2 acceleration{ 0.0f, 0.0f };

	void Move(glm::vec2& actorVelocity, const glm::vec2& acceleration);

	void WallJump();

	void Jump(float& deltaTime, float& foo);

	bool debugMove{ false };

	const int jumpBufferTime{ 100 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpBufferTimer;

	const int jumpTime{ 400 };
	std::chrono::time_point < std::chrono::steady_clock, std::chrono::duration<long long, std::ratio < 1, 1000000000>>> jumpTimer;

	bool isJumping{ false };

	float foo;

	bool gstate;

public:
	MovementHandler();

	~MovementHandler();

	void Update(float& deltaTime);

	bool spacebarOneShot{ true };


	bool KeyboadStates[static_cast<int>(MovementState::MOVE_DOWN)] = { false };

	bool LeftWallHug{ false };
	bool RightWallHug{ false };

	bool GravityState{ true };

};


