#pragma once
#include "Sprite.hpp"
#include <glm/glm.hpp>
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

	glm::mat4 mModelMatrix{ glm::mat4(1.0f) }; 

	glm::vec2 mTriggerAABBPos{ 0.0f, 0.0f };
	glm::vec2 mTriggerAABBSize{ 0.0f, 0.0f };

	float flyAngleTarget{ -1.0f };

	float flyAngle{ 0.0f };

	bool isDestroyed{ false };

	bool isVisible{ false }; 

	bool isSucked{ false };

	bool isDeathTrigger{ false };

	bool isConsumedByVoid{ false };

	bool isCollidable{ false };
};