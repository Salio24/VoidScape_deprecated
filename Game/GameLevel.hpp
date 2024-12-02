#pragma once
#include <vector>
#include "GameObject.hpp"
#include <nlohmannjson/json.hpp>
#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class GameLevel {
public:
	GameLevel();

	~GameLevel();

	//nlohmann::json GameLevelDataJson;

	std::vector<GameObject> mBlocks;

	std::vector<std::vector<std::string>> mLevelDataCsv;

	std::vector<std::vector<std::vector<int>>> mLevelData;

	std::vector<int> mTilesetsOffsets;

	void LoadLevel(const std::string file);

	void LoadLevelJson(const std::string file);

	void BuildLevel();

	void BuildLevelOld();

	bool isLoaded{ false };

	int mLevelWidth;
	int mLevelHeight;

	uint8_t mBlockSize{ 18 };

};