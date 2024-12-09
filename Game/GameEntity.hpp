#pragma once
#include "Sprite.hpp"
#include <glm/vec2.hpp>
#include "GameObject.hpp"

class GameEntity : public GameObject {
public:
	GameEntity();

	~GameEntity();

	virtual void Update();

	float movementSpeed{ 300.0f };

	glm::vec2 velocity{ 0.0f, 0.0f };

	void SetSprite(const Sprite& sprite);
};