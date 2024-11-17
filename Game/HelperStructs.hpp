#pragma once
#include <glm/glm.hpp>

struct Vertex {
	glm::vec2 Position;
	glm::vec3 Color;
	glm::vec2 Size;
};

enum class MovementState {
	MOVE_LEFT,
	MOVE_RIGHT,
	SPACE,
	MOVE_DOWN
};
