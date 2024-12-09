#include "Actor.hpp"

Actor::Actor() {

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
	mRelativePosition = mPosition - mSprite.vertexData.Position;
}