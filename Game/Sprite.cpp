#include "Sprite.hpp"

Sprite::Sprite() {
	//vertex[0].Position = glm::vec2(-5.0f, -5.0f);
	//vertex[1].Position = glm::vec2(5.0f, -5.0f);
	//vertex[2].Position = glm::vec2(5.0f, 5.0f);
	//vertex[3].Position = glm::vec2(-5.0f, 5.0f);
	////vertex[0].Color = glm::vec3(0.0f, 1.0f, 0.0f);
	////vertex[1].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	////vertex[2].Color = glm::vec3(0.0f, 0.0f, 1.0f);
	////vertex[3].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	//vertex[0].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	//vertex[1].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	//vertex[2].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	//vertex[3].Color = glm::vec3(1.0f, 0.0f, 0.0f);
	vertexData.Position = glm::vec2(0.0f, 0.0f);
	vertexData.Size = glm::vec2(0.0f, 0.0f);
	vertexData.Color = glm::vec3(0.0f, 0.0f, 0.0f);
	std::cout << "Sprite created" << std::endl;
}

Sprite::~Sprite() {

}