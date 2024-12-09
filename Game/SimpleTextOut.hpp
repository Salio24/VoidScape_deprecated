#pragma once
#include <SDL3/SDL.h>
#include "Texture.hpp"
#include "glm/glm.hpp"
#include <vector>

class TextOut {
public:
	TextOut();

	~TextOut();

	void Init(TextureHandler& textureHandler, const char* filepath);

	void Update();

	int mTextTextureIndex{ 0 };

	std::vector<glm::vec2> mTexturePositions;

	glm::vec2 mTextureSize{ 0.0f, 0.0f };
};