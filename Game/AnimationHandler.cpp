#include "AnimationHandler.hpp"

AnimationHandler::AnimationHandler(TextureHandler& textureHandlerRef) : textureHandlerRef(textureHandlerRef) {

}

AnimationHandler::~AnimationHandler() {

}

void AnimationHandler::Init(const int& samplerSize) {
	sampelrSize = samplerSize;

	SDL_Surface* DeadAnimationSurface           = textureHandlerRef.LoadSurface("assets/Actor/DeathAnimation.png");
	SDL_Surface* DuckAnimationSurface           = textureHandlerRef.LoadSurface("assets/Actor/DuckAnimation.png");
	SDL_Surface* HitAnimationSurface            = textureHandlerRef.LoadSurface("assets/Actor/HitAnimation.png");
	SDL_Surface* RunAnimationSurface            = textureHandlerRef.LoadSurface("assets/Actor/RunAnimation.png");
	SDL_Surface* FallAnimationSurface           = textureHandlerRef.LoadSurface("assets/Actor/FallAnimation.png");
	SDL_Surface* IdleAnimationSurface           = textureHandlerRef.LoadSurface("assets/Actor/IdleAnimation.png");
	SDL_Surface* JumpAnimationSurface	        = textureHandlerRef.LoadSurface("assets/Actor/JumpAnimation.png");
	SDL_Surface* SlamAnimationSurface           = textureHandlerRef.LoadSurface("assets/Actor/SlamAnimation.png");
	SDL_Surface* SlideAnimationSurface          = textureHandlerRef.LoadSurface("assets/Actor/SlideAnimation.png");
	SDL_Surface* WallSlideAnimationSurface      = textureHandlerRef.LoadSurface("assets/Actor/WallSlideAnimation.png");
	SDL_Surface* DuckIdleAnimationSurface       = textureHandlerRef.LoadSurface("assets/Actor/DuckIdleAnimation.png");
	SDL_Surface* BlackHoleBirthAnimationSurface = textureHandlerRef.LoadSurface("assets/Level/BlackHole_Birth_Anim.png");
	SDL_Surface* BlackHoleLoopAnimationSurface  = textureHandlerRef.LoadSurface("assets/Level/BlackHole_Loop_Anim.png");
	SDL_Surface* EscapePortalAnimationSurface   = textureHandlerRef.LoadSurface("assets/Level/Portal.png");

	InitMultiFrameAnimation(DeadAnimationSurface, DeadAnimation, glm::vec2(84.0f, 68.0f), 700, animationSpriteSizeX, animationSpriteSizeY);
	DeadAnimation.SingleFrameTime = DeadAnimation.AnimationTime / DeadAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(DuckAnimationSurface, DuckAnimation, glm::vec2(52.0f, 56.0f), 1000, animationSpriteSizeX, animationSpriteSizeY);
	DuckAnimation.SingleFrameTime = DuckAnimation.AnimationTime / DuckAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(HitAnimationSurface, HitAnimation, glm::vec2(60.0f, 68.0f), 300, animationSpriteSizeX, animationSpriteSizeY);
	HitAnimation.SingleFrameTime = HitAnimation.AnimationTime / HitAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(RunAnimationSurface, RunAnimation, glm::vec2(56.0f, 76.0f), 500, animationSpriteSizeX, animationSpriteSizeY);
	RunAnimation.SingleFrameTime = RunAnimation.AnimationTime / RunAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(BlackHoleBirthAnimationSurface, BlackHoleBirthAnimation, glm::vec2(332.0f, 332.0f), 400, 512, 512);
	BlackHoleBirthAnimation.SingleFrameTime = BlackHoleBirthAnimation.AnimationTime / BlackHoleBirthAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(BlackHoleLoopAnimationSurface, BlackHoleLoopAnimation, glm::vec2(332.0f, 332.0f), 800, 512, 512);
	BlackHoleLoopAnimation.SingleFrameTime = BlackHoleLoopAnimation.AnimationTime / BlackHoleLoopAnimation.AnimationTextureIndexes.size();

	InitMultiFrameAnimation(EscapePortalAnimationSurface, EscapePortalAnimation, glm::vec2(264.0f, 496.0f), 900, 512, 512);
	EscapePortalAnimation.SingleFrameTime = EscapePortalAnimation.AnimationTime / EscapePortalAnimation.AnimationTextureIndexes.size();

	InitSingleFrameAnimation(FallAnimationSurface, FallAnimation, glm::vec2(56.0f, 72.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(IdleAnimationSurface, IdleAnimation, glm::vec2(60.0f, 68.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(JumpAnimationSurface, JumpAnimation, glm::vec2(60.0f, 72.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(SlamAnimationSurface, SlamAnimation, glm::vec2(60.0f, 72.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(SlideAnimationSurface, SlideAnimation, glm::vec2(76.0f, 52.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(WallSlideAnimationSurface, WallSlideAnimation, glm::vec2(56.0f, 76.0f), animationSpriteSizeX, animationSpriteSizeY);
	InitSingleFrameAnimation(DuckIdleAnimationSurface, DuckIdleAnimation, glm::vec2(48.0f, 56.0f), animationSpriteSizeX, animationSpriteSizeY);

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
	SDL_DestroySurface(BlackHoleBirthAnimationSurface);
	SDL_DestroySurface(BlackHoleLoopAnimationSurface);
	SDL_DestroySurface(EscapePortalAnimationSurface);

}

void AnimationHandler::InitMultiFrameAnimation(SDL_Surface* animationSpritesheet, Animation& animation, const glm::vec2& animationSize, const int& animationTime, const int& spriteSizeX, const int& spriteSizeY, const glm::vec2& texturePosition) {
	std::vector<SDL_Surface*> tiles = textureHandlerRef.CutTileset(animationSpritesheet, spriteSizeX, spriteSizeY);
	for (int i = 0; i < tiles.size(); i++) {
		animation.AnimationTextureIndexes.push_back((uint32_t)textureHandlerRef.mLayersUsed[0]);
		textureHandlerRef.LoadTexture(tiles[i], GL_RGBA, textureHandlerRef.mLayersUsed[0], 0);
		SDL_DestroySurface(tiles[i]);
	}
	animation.TexturePosition = texturePosition;
	animation.Size = glm::vec2(animationSize.x / (float)sampelrSize, animationSize.y / (float)sampelrSize);
	animation.AnimationTime = animationTime;
	SDL_DestroySurface(animationSpritesheet);
}

void AnimationHandler::InitSingleFrameAnimation(SDL_Surface* animationSprite, Animation& animation, const glm::vec2& animationSize, const int& spriteSizeX, const int& spriteSizeY, const glm::vec2& texturePosition) {
	animation.AnimationTextureIndexes.push_back((uint32_t)textureHandlerRef.mLayersUsed[0]);
	textureHandlerRef.LoadTexture(textureHandlerRef.FlipSurfaceVertically(animationSprite), GL_RGBA, textureHandlerRef.mLayersUsed[0], 0);
	animation.TexturePosition = texturePosition;
	animation.Size = glm::vec2(animationSize.x / (float)sampelrSize, animationSize.y / (float)sampelrSize);
	animation.AnimationTime = 0.0f;
	SDL_DestroySurface(animationSprite);
}