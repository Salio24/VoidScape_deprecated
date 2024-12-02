#include "GameObject.hpp"

GameObject::GameObject() {

}

GameObject::~GameObject() {
	//std::cout << "Specs: " << "(isDestroyed: " << isDestroyed << "; isVisible: " << isVisible << "; isSucked: " << isSucked << "; isCollidable: " << isCollidable << ")  " << "Velocity:  "  << tempVelocity.x <<  ", " << tempVelocity.y << ":" << std::endl;
}

void GameObject::Update() {
	mPosition = mSprite.vertexData.Position;
}