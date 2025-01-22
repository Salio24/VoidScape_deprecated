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

	//std::vector<GameObject> mBlocks;

	void LoadLevelJson(const std::string file);

	void BuildLevel(const int& tilesetOffset);

private:
	std::vector<int> tilesetsOffsets;

	std::vector<std::vector<std::string>> levelDataCsv;

	std::vector<std::vector<std::vector<int>>> levelData;

	int levelWidth;
	int levelHeight;

	uint8_t blockSize{ 18 };
};