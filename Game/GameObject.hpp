#pragma once
#include "Sprite.hpp"
#include <iostream>


class GameObject {
public:
	GameObject();
	
	virtual ~GameObject();

	virtual void Update();

	Sprite mSprite;

	glm::vec2 mPosition{ 0.0f, 0.0f };

	glm::vec2 tempVelocity{ 0.0f, 0.0f };

	glm::vec2 flyDirectionNormalized{ 0.0f, 0.0f };

	// in radians
	float flyAngleTarget{ 0.0f };

	float flyAngle{ 0.0f };

	bool isDestroyed{ false };

	bool isVisible{ false }; 

	bool isSucked{ false };

	bool isCollidable{ false };
};