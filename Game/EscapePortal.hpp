#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "GameObject.hpp"
#include "AnimationHandler.hpp"
#include "glm/glm.hpp"
#include "Actor.hpp"
#include "CollisionHandler.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <SDL3/SDL_mixer.h>

class EscapePortal : public GameObject {
public:
	EscapePortal();

	~EscapePortal();

	void Update(Animation& portalAnim, const float& deltaTime, Actor& actor, Mix_Chunk* portalSoundEscape, Mix_Chunk* portalSoundIdle);

	glm::vec2 AnimationSize{ 0.0f, 0.0f };

	float sizeMultiplier{ 200.0f };


	bool animationOneShot{ true };

	glm::vec2 AABBPos{ 0.0f, 0.0f };
	glm::vec2 AABBSize{ 0.0f, 0.0f };
	float suctionVelocity{ 0.0f };

	glm::vec2 EpicenterAABBPos{ 0.0f, 0.0f };
	glm::vec2 EpicenterAABBSize{ 0.0f, 0.0f };

private:
	int idleVolume{ 0 };
	// squared distance of idle volume range
	float idleRadius{ 1250000.0f };

};