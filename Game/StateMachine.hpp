#pragma once
#include "Actor.hpp"
#include <glm/glm.hpp>
#include "MovementHandler.hpp"
#include "AnimationHandler.hpp"
#include "AudioHandler.hpp"

enum class PlayerStates {
	RUNNING,
	JUMPING,
	DOUBLE_JUMPING,
	FALLING,
	SLIDING,
	SLAMMING,
	WALLSLIDING,
	DEAD,
	HIT,
	IDLE,
	SUCKED,
	VOID_CONSUMED,
	ESCAPED,
	END
};

class StateMachine {
public:
	StateMachine();

	~StateMachine();

	void Update(MovementHandler& movementHandler, AnimationHandler& animationHandler, AudioHandler& audioHandler, Actor& actor, const float& deltaTime);

	bool CheckPlayerStateChange();

	bool CompareToLastState(const PlayerStates& state);

	glm::vec2 mCurrentActorDrawSize{ 0.0f, 0.0f };
	glm::vec2 mCurrentActorTextureSize{ 0.0f, 0.0f };
	glm::vec2 mCurrentActorTexturePosition{ 0.0f, 0.0f };
	uint32_t mCurrentActorTextureIndex{ 0 };
	bool mActorFlipped{ false };

	PlayerStates currentPlayerState = PlayerStates::IDLE;
	PlayerStates lastState = PlayerStates::IDLE;
	bool deadAnimOneShot{ true };
	bool deadAnimDone{ false };
private:

	int FallVolume{ 1 };
	bool runAnimationOneShot{ true };
	bool playingRunSound{ false };
	float FallVolumeTime{ 0.1f };
	float FallVolumeTimer{ 0.0f };

	int SuckedVolume{ 1 };
	float SuckedVolumeTime{ 0.2f };
	float SuckedVolumeTimer{ 0.0f };

	void CheckPlayerState(Actor& actor, MovementHandler& movementHandler);
};