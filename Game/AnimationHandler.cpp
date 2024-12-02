#include "AnimationHandler.hpp"

AnimationHandler::AnimationHandler(TextureHandler& mTextureHandlerRef) : mTextureHandlerRef(mTextureHandlerRef) {

}

AnimationHandler::~AnimationHandler() {

}

void AnimationHandler::Init() {
	SDL_Surface* DeadAnimationSurface      = mTextureHandlerRef.LoadSurface("assets/Actor/DeadAnimation.png");
	SDL_Surface* DuckAnimationSurface      = mTextureHandlerRef.LoadSurface("assets/Actor/DuckAnimation.png");
	SDL_Surface* HitAnimationSurface       = mTextureHandlerRef.LoadSurface("assets/Actor/HitAnimation.png");
	SDL_Surface* RunAnimationSurface       = mTextureHandlerRef.LoadSurface("assets/Actor/RunAnimation.png");
	SDL_Surface* FallAnimationSurface      = mTextureHandlerRef.LoadSurface("assets/Actor/FallAnimation.png");
	SDL_Surface* IdleAnimationSurface      = mTextureHandlerRef.LoadSurface("assets/Actor/IdleAnimation.png");
	SDL_Surface* JumpAnimationSurface	   = mTextureHandlerRef.LoadSurface("assets/Actor/JumpAnimation.png");
	SDL_Surface* SlamAnimationSurface      = mTextureHandlerRef.LoadSurface("assets/Actor/SlamAnimation.png");
	SDL_Surface* SlideAnimationSurface     = mTextureHandlerRef.LoadSurface("assets/Actor/SlideAnimation.png");
	SDL_Surface* WallSlideAnimationSurface = mTextureHandlerRef.LoadSurface("assets/Actor/WallSlideAnimation.png");
	SDL_Surface* DuckIdleAnimationSurface  = mTextureHandlerRef.LoadSurface("assets/Actor/DuckIdleAnimation.png");

	InitMultiFrameAnimation(DeadAnimationSurface, DeadAnimation, glm::vec2(21.0f, 17.0f), 700, AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitMultiFrameAnimation(DuckAnimationSurface, DuckAnimation, glm::vec2(13.0f, 14.0f), 1000, AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitMultiFrameAnimation(HitAnimationSurface,  HitAnimation,  glm::vec2(15.0f, 17.0f), 300, AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitMultiFrameAnimation(RunAnimationSurface,  RunAnimation,  glm::vec2(14.0f, 19.0f), 500, AnimationSpriteSizeX, AnimationSpriteSizeY);
	RunAnimation.SingleFrameTime = RunAnimation.AnimationTime / RunAnimation.AnimationTextureIndexes.size();

	InitSingleFrameAnimation(FallAnimationSurface,      FallAnimation,      glm::vec2(14.0f, 18.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(IdleAnimationSurface,      IdleAnimation,      glm::vec2(15.0f, 17.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(JumpAnimationSurface,      JumpAnimation,      glm::vec2(15.0f, 18.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(SlamAnimationSurface,      SlamAnimation,      glm::vec2(15.0f, 18.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(SlideAnimationSurface,     SlideAnimation,     glm::vec2(19.0f, 13.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(WallSlideAnimationSurface, WallSlideAnimation, glm::vec2(14.0f, 19.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);
	InitSingleFrameAnimation(DuckIdleAnimationSurface,  DuckIdleAnimation,  glm::vec2(12.0f, 14.0f), AnimationSpriteSizeX, AnimationSpriteSizeY);

	SDL_DestroySurface(DeadAnimationSurface);
	SDL_DestroySurface(DuckAnimationSurface);
	SDL_DestroySurface(HitAnimationSurface);
	SDL_DestroySurface(RunAnimationSurface);
	SDL_DestroySurface(FallAnimationSurface);
	SDL_DestroySurface(IdleAnimationSurface);
	SDL_DestroySurface(JumpAnimationSurface);
	SDL_DestroySurface(SlamAnimationSurface);
	SDL_DestroySurface(SlideAnimationSurface);
	SDL_DestroySurface(WallSlideAnimationSurface);
	SDL_DestroySurface(DuckIdleAnimationSurface);

}

void AnimationHandler::InitMultiFrameAnimation(SDL_Surface* animationSpritesheet, Animation& animation, const glm::vec2& animationSize, const int& animationTime, const int& spriteSizeX, const int& spriteSizeY, const glm::vec2& texturePosition) {
	std::vector<SDL_Surface*> tiles = mTextureHandlerRef.CutTileset(animationSpritesheet, spriteSizeX, spriteSizeY);
	for (int i = 0; i < tiles.size(); i++) {
		animation.AnimationTextureIndexes.push_back((uint32_t)mTextureHandlerRef.layersUsed[0]);
		mTextureHandlerRef.LoadTexture(tiles[i], GL_RGBA, mTextureHandlerRef.layersUsed[0], 0);
		SDL_DestroySurface(tiles[i]);
	}
	animation.TexturePosition = texturePosition;
	animation.Size = glm::vec2(animationSize.x / (float)spriteSizeX, animationSize.y / (float)spriteSizeY);
	animation.AnimationTime = animationTime;
	SDL_DestroySurface(animationSpritesheet);
}

void AnimationHandler::InitSingleFrameAnimation(SDL_Surface* animationSprite, Animation& animation, const glm::vec2& animationSize, const int& spriteSizeX, const int& spriteSizeY, const glm::vec2& texturePosition) {
	animation.AnimationTextureIndexes.push_back((uint32_t)mTextureHandlerRef.layersUsed[0]);
	mTextureHandlerRef.LoadTexture(mTextureHandlerRef.FlipSurfaceVertically(animationSprite), GL_RGBA, mTextureHandlerRef.layersUsed[0], 0);
	animation.TexturePosition = texturePosition;
	animation.Size = glm::vec2(animationSize.x / (float)spriteSizeX, animationSize.y / (float)spriteSizeY);
	animation.AnimationTime = 0.0f;
	SDL_DestroySurface(animationSprite);
}