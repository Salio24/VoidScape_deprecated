#include "Texture.hpp"

TextureHandler::TextureHandler() {
	
}

TextureHandler::~TextureHandler() {
	
}

SDL_Surface* TextureHandler::LoadSurface(const char* filepath) {
	SDL_Surface* surface = IMG_Load(filepath);
	if (!surface) {
		std::cerr << "IMG_Load Error: Cannot load " << filepath << std::endl;
		exit(1);
	}
	return surface;
}

void TextureHandler::InitTextureArray(const GLenum& internalformat, const GLsizei& width, const GLsizei& height, const GLsizei& depth) {
	glGenTextures(1, &textureArrays[TextureSlotsTaken]);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[TextureSlotsTaken]);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalformat, width, height, depth);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    layersUsed[TextureSlotsTaken] = 0;
	TextureSlotsTaken++;
}

void TextureHandler::LoadTexture(SDL_Surface* surface, const GLenum& internalformat, const int& layer, const int& slot) {
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, surface->w, surface->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_DestroySurface(surface);
    layersUsed[slot]++;
}

std::vector<SDL_Surface*> TextureHandler::CutTileset(SDL_Surface* tileset, const int& tileWidth, const int& tileHeight) {

    std::vector<SDL_Surface*> tiles;

    // Calculate the number of rows and columns in the tileset
    int rows = tileset->h / tileHeight;
    int cols = tileset->w / tileWidth;

    tileset = SDL_ConvertSurface(tileset, SDL_PIXELFORMAT_ABGR8888);
    if (!tileset) {
        SDL_Log("Failed to convert tileset format: %s", SDL_GetError());
    }

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // Define the rectangle for the current tile
            SDL_Rect srcRect = { col * tileWidth, row * tileHeight, tileWidth, tileHeight };    


            // Create a new surface for the tile
            SDL_Surface* tile = SDL_CreateSurface(tileWidth, tileHeight, tileset->format);
            if (!tile) {
                std::cerr << "Failed to create tile surface: " << SDL_GetError() << std::endl;
                continue;
            }

            //std::cout << "col: " << col << ", " << "row:  " << row << std::endl;
            // Copy the tile's pixels from the tileset to the new surface
            SDL_BlitSurface(tileset, &srcRect, tile, NULL);
            tiles.push_back(FlipSurfaceVertically(tile));
            SDL_DestroySurface(tile);
        }
    }
    SDL_DestroySurface(tileset);

    return tiles;
}

SDL_Surface* TextureHandler::FlipSurfaceVertically(SDL_Surface* surface) {
    // Create a new surface with the same dimensions as the original surface
    SDL_Surface* flipped = SDL_CreateSurface(surface->w, surface->h, surface->format);

    if (flipped == nullptr) {
        SDL_Log("Unable to create surface: %s", SDL_GetError());
        return nullptr;
    }

    // Create source and destination rectangles
    SDL_Rect srcRect = { 0, 0, surface->w, 1 };  // Only one row at a time
    SDL_Rect dstRect = { 0, 0, surface->w, 1 };  // Only one row at a time

    // Iterate through each row from bottom to top
    for (int y = 0; y < surface->h; ++y) {
        // Set the source y position (starting from the bottom row)
        srcRect.y = surface->h - y - 1;
        // Set the destination y position (starting from the top row)
        dstRect.y = y;

        // Copy the row from the original surface to the new flipped surface
        SDL_BlitSurface(surface, &srcRect, flipped, &dstRect);
    }

    return flipped;
}