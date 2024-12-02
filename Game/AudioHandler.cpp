#include "AudioHandler.hpp"

AudioHandler::AudioHandler() {
}

AudioHandler::~AudioHandler() {
	Mix_FreeChunk(Jump);
	Mix_FreeChunk(DoubleJump);
	//Mix_FreeChunk(Land);
	Mix_FreeChunk(Slam);
	Mix_FreeChunk(Slide);
	Mix_FreeChunk(WallSlide);
	Mix_FreeChunk(WindHard);
	Mix_FreeChunk(WindSoft);

	Mix_FreeChunk(RunSounds[0]);
	Mix_FreeChunk(RunSounds[1]);
	Mix_FreeChunk(RunSounds[2]);
	Mix_FreeChunk(RunSounds[3]);
	Mix_FreeChunk(RunSounds[4]);
	Mix_FreeChunk(RunSounds[5]);
	Mix_FreeChunk(RunSounds[6]);
	Mix_FreeChunk(RunSounds[7]);
	Mix_FreeChunk(RunSounds[8]);
	Mix_FreeChunk(RunSounds[9]);

	Mix_FreeChunk(LandSoftSounds[0]);
	Mix_FreeChunk(LandSoftSounds[1]);
	Mix_FreeChunk(LandSoftSounds[2]);
	Mix_FreeChunk(LandSoftSounds[3]);
	Mix_FreeChunk(LandSoftSounds[4]);

	Mix_FreeChunk(LandHardSounds[0]);
}

void AudioHandler::LoadSounds() {
	Jump =         Mix_LoadWAV("assets/Sounds/FerrymanSwing1.wav");
	DoubleJump =   Mix_LoadWAV("assets/Sounds/FerrymanSwing2.wav");
	//Land =         Mix_LoadWAV("assets/Sounds/land/phys_imp_bodyfall_human_soft_1ch_v2_02.wav");
	Slam =         Mix_LoadWAV("assets/Sounds/Landing.wav");
	Slide =        Mix_LoadWAV("assets/Sounds/SlideGrass2Edit.wav");
	WallSlide =    Mix_LoadWAV("assets/Sounds/SlideGrass3Edit.wav");
	WindHard = Mix_LoadWAV("assets/Sounds/WindHard.wav");
	WindSoft = Mix_LoadWAV("assets/Sounds/WindSoft.wav");

	RunSounds[0] = Mix_LoadWAV("assets/Sounds/run_default_01.wav");
	RunSounds[1] = Mix_LoadWAV("assets/Sounds/run_default_02.wav");
	RunSounds[2] = Mix_LoadWAV("assets/Sounds/run_default_03.wav");
	RunSounds[3] = Mix_LoadWAV("assets/Sounds/run_default_04.wav");
	RunSounds[4] = Mix_LoadWAV("assets/Sounds/run_default_05.wav");
	RunSounds[5] = Mix_LoadWAV("assets/Sounds/run_default_06.wav");
	RunSounds[6] = Mix_LoadWAV("assets/Sounds/run_default_07.wav");
	RunSounds[7] = Mix_LoadWAV("assets/Sounds/run_default_08.wav");
	RunSounds[8] = Mix_LoadWAV("assets/Sounds/run_default_09.wav");
	RunSounds[9] = Mix_LoadWAV("assets/Sounds/run_default_10.wav");

	LandSoftSounds[0] = Mix_LoadWAV("assets/Sounds/LandSoft1.wav");
	LandSoftSounds[1] = Mix_LoadWAV("assets/Sounds/LandSoft2.wav");
	LandSoftSounds[2] = Mix_LoadWAV("assets/Sounds/LandSoft3.wav");
	LandSoftSounds[3] = Mix_LoadWAV("assets/Sounds/LandSoft4.wav");
	LandSoftSounds[4] = Mix_LoadWAV("assets/Sounds/LandSoft5.wav");

	LandHardSounds[0] = Mix_LoadWAV("assets/Sounds/LandHard5.wav");

}

void AudioHandler::PlayNextStepSound() {
	if (runSoundIndex == 10) {
		runSoundIndex = 0;
	}
	Mix_PlayChannel(1, RunSounds[runSoundIndex], 0);
	runSoundIndex++;
}

void AudioHandler::PlayNextLandSoftSound() {
	if (landSoftSoundIndex == 5) {
		landSoftSoundIndex = 0;
	}
	Mix_PlayChannel(7, LandSoftSounds[landSoftSoundIndex], 0);
	landSoftSoundIndex++;
}

void AudioHandler::PlayNextLandHardSound() {
	if (landHardSoundIndex == 1) {
		landHardSoundIndex = 0;
	}
	Mix_PlayChannel(7, LandHardSounds[landHardSoundIndex], 0);
	landHardSoundIndex++;
}