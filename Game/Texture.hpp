#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_surface.h>
#include <string>
#include <array>
#include <glad/glad.h>
#include <iostream>
#include <vector>


class TextureHandler {
public:
	TextureHandler();

	~TextureHandler();

	SDL_Surface* LoadSurface(const char* filepath);

	void InitTextureArray(const GLenum& internalformat, const GLsizei& width, const GLsizei& height, const GLsizei& depth);

	void LoadTexture(SDL_Surface* surface, const GLenum& internalformat, const int& layer, const int& slot);

	SDL_Surface* FlipSurfaceVertically(SDL_Surface* surface);

	void CutAndLoad(const char* filepath, int& offsetValue, const int& arrayIndex, const int& resolution);

	void LoadAll(const int& arrayIndex = 1);

	std::vector<SDL_Surface*> CutTileset(SDL_Surface* tileset, const int& tileWidth, const int& tileHeight);

	// array to keep track how many layers are in use
	std::array<int, 32> mLayersUsed;

	// offset of a base tileset textures in texture array
	int mBaseT_Offset{ 0 };
	// offset of UI borders tileset textures in texture array
	int mUI_BordersT_Offset{ 0 };
	// offset of UI arrows tileset textures
	int mUI_ArrowsT_Offset{ 0 };
	// offset of Orb tileset textures
	int mOrbT_Offset{ 0 };


private:
	std::array<GLuint, 32> textureArrays;

	int TextureSlotsTaken = 0;
};