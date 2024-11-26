#pragma once
#include <glm/glm.hpp>

struct Box {
	glm::vec2 Position;
	glm::vec4 Color;
	glm::vec2 Size;
	glm::vec2 TexturePosition;
	int TextureIndex;
};

enum class MovementState {
	MOVE_LEFT,
	MOVE_RIGHT,
	SPACE,
	DUCK,
	MOVE_UP,
	MOVE_DOWN,
	END
};
