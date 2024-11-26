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

	void LoadTexture(SDL_Surface* surface, const GLenum& internalformat, const int& layer);

	SDL_Surface* FlipSurfaceVertically(SDL_Surface* surface);
	std::vector<SDL_Surface*> CutTileset(SDL_Surface* tileset, int tileWidth, int tileHeight);

	int TextureSlotsTaken = 0;

	std::array<GLuint, 32> textureArrays;

	// array to keep track how many layers are in use
	std::array<int, 32> layersUsed;

	

};