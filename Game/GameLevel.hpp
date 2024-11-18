#pragma once
#include <vector>
#include "GameObject.hpp"
#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class GameLevel {
public:
	GameLevel();

	~GameLevel();

	std::vector<GameObject> mBlocks;

	std::vector<std::vector<std::string>> mLevelData;

	void LoadLevel(const std::string file);

	void BuildLevel();

	bool isLoaded{ false };

	uint16_t mLevelWidth;
	uint16_t mLevelHeight;

	uint8_t mBlockSize{ 18 };

};