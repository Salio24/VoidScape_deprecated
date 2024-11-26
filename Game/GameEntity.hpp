#pragma once
#include "Sprite.hpp"
#include <glm/vec2.hpp>
#include "GameObject.hpp"

class GameEntity : public GameObject {
public:
	GameEntity();

	~GameEntity();

	virtual void Render();

	virtual void Transform();
	GLuint mVAO;

	float movementSpeed{ 300.0f };

	bool isGrounded{ false };

	bool isWallMountable{ false };
	bool isWallMountableL{ false };
	bool isWallMountableR{ false };

	glm::vec2 velocity{ 0.0f, 0.0f };

	void SetSprite(const Sprite& sprite);

	glm::mat4 mModelMatrix{ glm::mat4(1.0f) };
};