#include "EscapePortal.hpp"

EscapePortal::EscapePortal() {
	suctionVelocity = 100.0f;
	mSprite.vertexData.Position = glm::vec2(17840.0f, 475.0f);
}

EscapePortal::~EscapePortal() {

}

void EscapePortal::Update(Animation& portalAnim, const float& deltaTime, Actor& actor, Mix_Chunk* portalSoundEscape, Mix_Chunk* portalSoundIdle) {
	if (animationOneShot) {
		EpicenterAABBPos = glm::vec2(mSprite.vertexData.Position.x + mSprite.vertexData.Size.x / 2.0f, mSprite.vertexData.Position.y + mSprite.vertexData.Size.y / 4);
		EpicenterAABBSize = glm::vec2(mSprite.vertexData.Size.x / 4, mSprite.vertexData.Size.y / 2);

		AABBSize = glm::vec2(250.0f, 250.0f);
		AABBPos = glm::vec2(EpicenterAABBPos.x + EpicenterAABBSize.x / 2.0f - AABBSize.x / 2.0f, EpicenterAABBPos.y + EpicenterAABBSize.y / 2.0f - AABBSize.y / 2.0f);

		AnimationSize = portalAnim.Size;
		mSprite.vertexData.TexturePosition = portalAnim.TexturePosition;
		portalAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		portalAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		animationOneShot = false;
	}

	if (RectVsRect(AABBPos, AABBSize, actor.mPosition, actor.mSprite.vertexData.Size) && actor.isSuckedPortal == false && actor.mEscaped == false) {
		actor.isCollidable = false;
		actor.isSuckedPortal = true;
		actor.flyDirectionNormalized = glm::normalize(EpicenterAABBPos + (EpicenterAABBSize / 3.0f) - actor.mPosition);
		actor.velocity = actor.flyDirectionNormalized * suctionVelocity;
	}
	if (actor.isSuckedPortal == true) {
		if (actor.mPosition.x >= EpicenterAABBPos.x + EpicenterAABBSize.x / 2) {
			actor.mEscaped = true;
			actor.isVisible = false;
			actor.isSuckedPortal = false;
			Mix_PlayChannel(9, portalSoundEscape, 0);
			actor.velocity = glm::vec2(0.0f, 0.0f);
		}

		actor.velocity += actor.velocity * 6.16f * deltaTime;

		if (actor.flyAngleTargetPortal == -1.0f) {
			float dot = glm::dot(actor.flyDirectionNormalized, glm::vec2(1.0f, 0.0f));
			actor.flyAngleTargetPortal = glm::acos(dot);
		}
		glm::vec2 center = glm::vec2(actor.mSprite.vertexData.Position.x + actor.mSprite.vertexData.Size.x / 2, actor.mSprite.vertexData.Position.y + actor.mSprite.vertexData.Size.y / 2);

		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(center, 0.0f));

		float crossProduct = glm::cross(actor.flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		if (crossProduct > 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, actor.flyAnglePortal, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (crossProduct < 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, -actor.flyAnglePortal, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(-center, 0.0f));

		if (actor.flyAnglePortal < actor.flyAngleTargetPortal) {
			actor.flyAnglePortal += 0.1f * deltaTime;
		}
	}

	float distance = glm::distance2(mSprite.vertexData.Position + mSprite.vertexData.Size / 2.0f, actor.mPosition + actor.mSprite.vertexData.Size / 2.0f);

	if (distance < idleRadius) {
		idleVolume = 128.0f - (distance / idleRadius * 128.0f);
		Mix_Volume(12, idleVolume);
		if (!Mix_Playing(12)) {
			Mix_PlayChannel(12, portalSoundIdle, 0);
		}
	}
	else {
		idleVolume = 0;
		Mix_Volume(12, idleVolume);
	}

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - portalAnim.AnimationTimer).count() > portalAnim.AnimationTime + deltaTime * 1000) {
		portalAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		portalAnim.index = 0;
	}
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - portalAnim.SingleFrameTimer).count() > portalAnim.SingleFrameTime + deltaTime * 1000) {
		portalAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		portalAnim.index++;
	}
	if (portalAnim.index > portalAnim.AnimationTextureIndexes.size() - 1) {
		portalAnim.index = 0;
	}
	mSprite.vertexData.TextureIndex = portalAnim.AnimationTextureIndexes[portalAnim.index];
}
