#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Box {
	glm::vec2 Position;
	glm::vec4 Color;
	glm::vec2 Size;
	glm::vec2 TexturePosition;
	int TextureIndex;
	float FlyAngle;
	float FlyOrientation;
	glm::vec2 Center;
};

struct Text {
	glm::vec2 Possition{0.0f, 0.0f};
	glm::vec3 Color{0.0f, 0.0f, 0.0f};
	std::string Text{""};
	glm::mat4 *ModelMatrix;
	bool CenteredX{ true };
	bool CenteredY{ true };
	bool RightSided{ false };
	float Scale;
};