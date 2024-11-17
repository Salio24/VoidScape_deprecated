#pragma once
#include "Sprite.hpp"


class GameObject {
public:
	GameObject();
	
	virtual ~GameObject();

	virtual void Update();

	Sprite mSprite;

	glm::vec2 mPosition{0.0f, 0.0f};

	bool isDestroyed{ false };

	bool isCollidable{ false };
};