#include "Sprite.hpp"

Sprite::Sprite() {
	mVertexData.Position = glm::vec2(0.0f, 0.0f);
	mVertexData.Size = glm::vec2(0.0f, 0.0f);
	mVertexData.Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

Sprite::~Sprite() {

}