#include "GameLevel.hpp"

GameLevel::GameLevel() {

}

GameLevel::~GameLevel() {

}

void GameLevel::LoadLevel(const std::string file) {

	this->mBlocks.clear();
	this->mLevelDataCsv.clear();

	std::string line;
	std::ifstream fstream(file);

	if (fstream) {
		// Read the entire file into a vector of strings
		std::vector<std::string> lines;
		while (std::getline(fstream, line)) {
			lines.push_back(line);
		}

		// Iterate over the vector in reverse order
		for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
			std::istringstream sstream(*it);
			std::string item;
			std::vector<std::string> row;
			while (std::getline(sstream, item, ',')) {
				row.push_back(item);
			}
			mLevelDataCsv.push_back(row);
		}
	}
	else {
		std::cout << "Could not load level data" << std::endl;
	}

	isLoaded = true;

}

void GameLevel::LoadLevelJson(const std::string file) {

	std::ifstream jsonStream(file);

	nlohmann::json GameLevelDataJson = nlohmann::json::parse(jsonStream);

	mLevelWidth = GameLevelDataJson["width"];
	mLevelHeight = GameLevelDataJson["height"];

	for (int i = 0; i < GameLevelDataJson["layers"].size(); i++) {
		if (GameLevelDataJson["layers"][i]["name"] == "BaseLayer") {
			std::vector<std::vector<int>> data(mLevelHeight, std::vector<int>(mLevelWidth));
			std::vector vec = GameLevelDataJson["layers"][i]["data"].get<std::vector<int>>();
			for (int j = 0; j < mLevelHeight; j++) {
				for (int k = 0; k < mLevelWidth; k++) {
					data[j][k] = vec[j * mLevelWidth + k];
				}
			}
			mTilesetsOffsets.push_back(GameLevelDataJson["tilesets"][i]["firstgid"]);
			mLevelData.push_back(data);
		}
		else if (GameLevelDataJson["layers"][i]["name"] == "CollisionLayer") {
			std::vector<std::vector<int>> data(mLevelHeight, std::vector<int>(mLevelWidth));
			std::vector vec = GameLevelDataJson["layers"][i]["data"].get<std::vector<int>>();
			for (int j = 0; j < mLevelHeight; j++) {
				for (int k = 0; k < mLevelWidth; k++) {
					data[j][k] = vec[j * mLevelWidth + k];
				}
			}
			mTilesetsOffsets.push_back(GameLevelDataJson["tilesets"][i]["firstgid"]);
			mLevelData.push_back(data);
		}
	}
}

void GameLevel::BuildLevel() {
	for (int i = 0; i < mLevelHeight; ++i) {
		for (int j = 0; j < mLevelWidth; ++j) {
			if (mLevelData[0][i][j] != 0 || mLevelData[1][i][j] != 0) {
				GameObject obj;
				obj.mSprite.vertexData.Position = glm::vec2(j * mBlockSize, ((mLevelHeight - i) * mBlockSize) - mBlockSize);
				obj.mSprite.vertexData.Size = glm::vec2(mBlockSize, mBlockSize);
				if (mLevelData[0][i][j] != 0) {
					obj.mSprite.vertexData.TextureIndex = mLevelData[0][i][j] - mTilesetsOffsets[0] + 1;
					obj.isVisible = true;
				}
				else {
					obj.isVisible = false;
				}
				if ((mLevelData[1][i][j] - mTilesetsOffsets[1]) + 1 == 1 || (mLevelData[1][i][j] - mTilesetsOffsets[1]) + 1 == 15) {
					obj.isCollidable = true;
				} 
				else {
					obj.isCollidable = false;
				}
				this->mBlocks.push_back(obj);
			}
		}
	}
}