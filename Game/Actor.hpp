#pragma once
#include "GameEntity.hpp"
#include <glm/vec2.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CollisionHandler.hpp"

enum class MovementDirection {
	None,
	Left,
	Right,
	Top,
	Bottom
};


class Actor : public GameEntity {
public:
	Actor();

	~Actor();

	void Update() override;

	void Move();

	void Transform();

	float movementSpeed{500.0f};

	MovementDirection Direction{ MovementDirection::None };

	glm::vec2 mRelativePosition{ 0.0f, 0.0f };

	glm::vec2 mScreenPosition{ 0.0f, 0.0f };

	float flyAngleTargetPortal{ -1.0f };
	float flyAnglePortal{ 0.0f };

	bool isWallMountableL{ false };
	bool isWallMountableR{ false };

	bool mEscaped{ false };

	bool mDead{ false };

	bool mDeadSoundOneShot{ true };

	bool isSuckedPortal{ false };

	float mSizeMultiplier{ 240.0f };
private:
};