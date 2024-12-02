#pragma once
#include <SDL3/SDL_mixer.h>
#include <array>


class AudioHandler {
public:
	AudioHandler();
	~AudioHandler();

	void LoadSounds();

	void PlayNextStepSound();

	void PlayNextLandSoftSound();

	void PlayNextLandHardSound();

	Mix_Chunk* Jump = NULL;
	bool JumpSoundOneShot{ true };
	Mix_Chunk* DoubleJump = NULL;
	bool DoubleJumpSoundOneShot{ true };
	//Mix_Chunk* Land = NULL;
	//bool LandSoundOneShot{ true };
	Mix_Chunk* Slam = NULL;
	bool SlamSoundOneShot{ true };
	Mix_Chunk* Slide = NULL;
	Mix_Chunk* WindHard = NULL;
	Mix_Chunk* WindSoft = NULL;
	Mix_Chunk* WallSlide = NULL;
	std::array<Mix_Chunk*, 10> RunSounds;
	std::array<Mix_Chunk*, 5> LandSoftSounds;
	std::array<Mix_Chunk*, 1> LandHardSounds;



private:

	int runSoundIndex{ 0 };
	int landSoftSoundIndex{ 0 };
	int landHardSoundIndex{ 0 };
};