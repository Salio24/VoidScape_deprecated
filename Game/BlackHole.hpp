#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "GameObject.hpp"
#include <random>
#include <vector>
#include "CollisionHandler.hpp"
#include "Actor.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/exterior_product.hpp>
#include "AnimationHandler.hpp"
#include <SDL3/SDL_mixer.h>

class BlackHole : public GameObject {
public:
	BlackHole();

	~BlackHole();

	void Update(std::vector<GameObject>& blocks, Actor& actor, const float& deltaTime, Animation& loopAnim, Animation& birthAnim, Mix_Chunk* bornSound, Mix_Chunk* consumedSound, Mix_Chunk* blackHoleIdle);

	glm::vec2 AnimationSize{ 0.0f, 0.0f };

	bool isBorn{ false };
	bool loopTimerOneShot{ false };
	bool birthTimerOneShot{ false };
	int idleVolume{ 0 };
	glm::vec2 AABBSize { 100.0f, 1080.0f };

private: 
	glm::vec2 AABBPos { 0.0f, 0.0f };

	glm::vec2 EpicenterAABBSize{ 40.0f, 40.0f };
	glm::vec2 EpicenterAABBPos{ 0.0f, 500.0f };
	float AABBVelocityMultiplier { 0.0f };
	// squared distance of idle volume range
	float idleRadius{ 1250000.0f };

	std::vector<std::pair<int, float>> affectedBlocks;

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> randomVelocity;
};