#include "Sprite.hpp"

Sprite::Sprite() {
	vertexData.Position = glm::vec2(0.0f, 0.0f);
	vertexData.Size = glm::vec2(0.0f, 0.0f);
	vertexData.Color = glm::vec3(0.0f, 0.0f, 0.0f);
	std::cout << "Sprite created" << std::endl;
}

Sprite::~Sprite() {

}