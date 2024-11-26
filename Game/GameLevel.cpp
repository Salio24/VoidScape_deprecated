#include "GameLevel.hpp"

GameLevel::GameLevel() {

}

GameLevel::~GameLevel() {

}

void GameLevel::LoadLevel(const std::string file) {

	this->mBlocks.clear();
	this->mLevelData.clear();

	std::string line;
	std::ifstream fstream(file);
	//if (fstream)
	//{
	//	while (std::getline(fstream, line)) // read each line from level file
	//	{
	//		std::istringstream sstream(line);
	//		std::string item;
	//		std::vector<std::string> row;
	//		while (std::getline(sstream, item, ',')) // read each word separated by spaces
	//			row.push_back(item);
	//		mLevelData.push_back(row);
	//	}
	//}
	//else {
	//	std::cout << "Could not load level data" << std::endl;
	//}

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
			mLevelData.push_back(row);
		}
	}
	else {
		std::cout << "Could not load level data" << std::endl;
	}

	isLoaded = true;

}

void GameLevel::BuildLevel() {
	for (int i = 0; i < mLevelData.size(); ++i) {
		for (int j = 0; j < mLevelData[i].size(); ++j) {
			//if (mLevelData[i][j] == "2") {
			//	GameObject obj;
			//	obj.mSprite.vertexData.Position = glm::vec2(j * mBlockSize, i * mBlockSize);
			//	obj.mSprite.vertexData.Size = glm::vec2(mBlockSize, mBlockSize);
			//	obj.mSprite.vertexData.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			//	this->mBlocks.push_back(obj);
			//}
			//if (mLevelData[i][j] == "1") {
			//	GameObject obj;
			//	obj.mSprite.vertexData.Position = glm::vec2(j * mBlockSize, i * mBlockSize);
			//	obj.mSprite.vertexData.Size = glm::vec2(mBlockSize, mBlockSize);
			//	obj.mSprite.vertexData.Color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			//	this->mBlocks.push_back(obj);
			//}
			//if (mLevelData[i][j] == "0") {
			//	GameObject obj;
			//	obj.mSprite.vertexData.Position = glm::vec2(j * mBlockSize, i * mBlockSize);
			//	obj.mSprite.vertexData.Size = glm::vec2(mBlockSize, mBlockSize);
			//	obj.mSprite.vertexData.Color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			//	this->mBlocks.push_back(obj);
			//}
			if (i < mLevelData.size() && j < mLevelData[i].size()) {
				if (mLevelData[i][j] != "-1") {
					GameObject obj;
					obj.mSprite.vertexData.Position = glm::vec2(j * mBlockSize, i * mBlockSize);
					obj.mSprite.vertexData.Size = glm::vec2(mBlockSize, mBlockSize);
					obj.mSprite.vertexData.TextureIndex = std::stoi(mLevelData[i][j]) + 1;
					this->mBlocks.push_back(obj);
				}
			}
			else {
				std::cerr << "Out of bounds: i = " << i << ", j = " << j << std::endl;
			}
		}
	}
}
