#pragma once
#include "SceneManager.hpp" 
#include <vector>

SceneManager::SceneManager() {
	mMainMenuLayoutPath = "assets/UI/Data/test40.json";
}

SceneManager::~SceneManager() {

}

void SceneManager::Update() {
	
}

void SceneManager::ResetState() {

}

// move gamelevel to here. rename to scenes*. combine LoadMenuLayout and LoadMainMenu.
// can obstract even further and separate with (UI scene(s) and game scene(s)) classes.

void SceneManager::LoadGameLevel(std::string levelPath, GameLevel* level, const int& tilesetOffset) {
	//level->mBlocks.clear();
	//level->LoadLevelJson(levelPath);
	//level->BuildLevel(tilesetOffset);
	//for (int i = 0; i < level->mBlocks.size(); i++) {
	//	level->mBlocks[i].Update();
	//}
	//mLevelActive = true;
}

void SceneManager::LoadMenuLayout(std::string layoutPath) {
	std::ifstream jsonStream(layoutPath);

	nlohmann::json GameLevelDataJson = nlohmann::json::parse(jsonStream);

	levelWidth = GameLevelDataJson["width"];
	levelHeight = GameLevelDataJson["height"];
	
	
	std::vector<std::vector<int>> data(levelHeight, std::vector<int>(levelWidth));
	std::vector vec = GameLevelDataJson["layers"][0]["data"].get<std::vector<int>>();
	for (int j = 0; j < levelHeight; j++) {
		for (int k = 0; k < levelWidth; k++) {
			data[j][k] = vec[j * levelWidth + k];
		}
	}
	offset = GameLevelDataJson["tilesets"][0]["firstgid"];
	layoutData.push_back(data);
}

void SceneManager::LoadMainMenu(const int& tilesetOffset) {

	mUIScenes.LoadMainMenuTab(tilesetOffset);
	mUIScenes.FillBlockVector();

	mCurrentBlocks = &mUIScenes.mMainMenuBlocks;
	mLevelActive = false;
	mMainMenuActive = true;
}

void SceneManager::LoadTitleScreen() {

}

void SceneManager::SceneChangeAnim(const float& deltaTime) {
	mChangingScene = true;

	if (!mCurtainDirection) {
		mCurtainSize.x += 5000.0f * deltaTime;
		if (mCurtainSize.x >= 1920.0f) {
			mCurtainSize.x = 1920.0f;
			mCurtainDirection = true;
		}
	}
	else {
		mCurtainSize.x -= 5000.0f * deltaTime;
		if (mCurtainSize.x <= 0.0f) {
			mCurtainSize.x = 0.0f;
			mCurtainDirection = false;
			mChangingScene = false;
		}
	}
}

void SceneManager::ReloadCurrentLevel(const int tilesetOffset, Mix_Music* introMusic) {
	switch (mCurrentLevel) {
	case Levels::LEVEL_1:
		mLevelScene.mLevelBlocks.clear();
		mLevelScene.LoadLevel("levels/GameLevels/32p/Level_1.json", tilesetOffset);
		mCurrentBlocks = &mLevelScene.mLevelBlocks;
		mLevelActive = true;
		mMainMenuActive = false;
		break;
	case Levels::TEST_LEVEL:
		mLevelScene.mLevelBlocks.clear();
		mLevelScene.LoadLevel("levels/GameLevels/32p/Test_Level.json", tilesetOffset);
		mCurrentBlocks = &mLevelScene.mLevelBlocks;
		mLevelActive = true;
		mMainMenuActive = false;
		break;
	default:
		std::cerr << "Error: ReloadCurrentLevel: Invalid current level" << std::endl;
		break;
	}
	Mix_HaltMusic();
	Mix_PlayMusic(introMusic, 0);
}

void SceneManager::LoadLevel(Levels level, const int tilesetOffset, Mix_Music* introMusic) {
	mCurrentLevel = level;
	ReloadCurrentLevel(tilesetOffset, introMusic);
}
