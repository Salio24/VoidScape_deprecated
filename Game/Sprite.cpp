#include "Sprite.hpp"

Sprite::Sprite() {
	vertexData.Position = glm::vec2(0.0f, 0.0f);
	vertexData.Size = glm::vec2(0.0f, 0.0f);
	vertexData.Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

Sprite::~Sprite() {

}