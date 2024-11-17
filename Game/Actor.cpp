#include "Actor.hpp"

Actor::Actor() {
	std::cout << "Actor created" << std::endl;
}

Actor::~Actor() {

}

void Actor::Transform() {
	mModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mRelativePosition, 0.0f));
}

void Actor::Update() {
	Move();
	Transform();
}

void Actor::Move() {
	//mPosition += CheckBorderColision(direction, mPosition) * mMovementSpeed;
	//mPosition = position + mSprite.vertexData.Position;

	mRelativePosition = mPosition - mSprite.vertexData.Position;
}