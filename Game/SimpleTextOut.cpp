#include "SimpleTextOut.hpp"

TextOut::TextOut() {

}

TextOut::~TextOut() {

}

void TextOut::Init(TextureHandler& textureHandler, const char* filepath) {
	SDL_Surface* surface = textureHandler.LoadSurface(filepath);

	surface = textureHandler.FlipSurfaceVertically(surface);

	mTextTextureIndex = textureHandler.layersUsed[0];

	textureHandler.LoadTexture(surface, GL_RGBA8, textureHandler.layersUsed[0], 0);
	SDL_DestroySurface(surface);

	mTextureSize = glm::vec2(1.0f, 31.0f / 512.0f);

	for (int i = 0; i < (int)512 / 31; i++) {
		mTexturePositions.push_back(glm::vec2(0.0f, (float)i * (31.0f / 512.0f)));
	}
}

void TextOut::Update() {

}