#include <string>
#include "GameLevel.hpp"
#include "GameObject.hpp"
#include "UIScenes.hpp"
#include "LevelScene.hpp"
#include <SDL3/SDL_mixer.h>

enum class Levels {
	LEVEL_1,
	TEST_LEVEL,
	END
};

class SceneManager {
public:
	SceneManager();

	~SceneManager();

	void Update();

	void ResetState();

	void LoadGameLevel(std::string levelPath, GameLevel* level, const int& tilesetOffset);

	void LoadMenuLayout(std::string layoutPath);

	void LoadMainMenu(const int& tilesetOffset);

	void LoadTitleScreen();

	void SceneChangeAnim(const float& deltaTime);

	void ReloadCurrentLevel(const int tilesetOffset, Mix_Music* introMusic);

	void LoadLevel(Levels level, const int tilesetOffset, Mix_Music* introMusic);

	bool mLevelActive{ false };

	bool mMainMenuActive{ false };

	int offset;

	int levelWidth;
	int levelHeight;

	UIScenes mUIScenes;

	LevelScene mLevelScene;

	Levels mCurrentLevel{ Levels::END };

	std::vector<GameObject> mBlocks;

	std::vector<GameObject>* mCurrentBlocks;

	std::vector<std::vector<std::vector<int>>> layoutData;

	std::string mMainMenuLayoutPath{ "" };

	bool mChangingScene{ false };

	bool mCurtainDirection{ false };

	glm::vec2 mCurtainSize{ 0.0f, 1080.0f };
};