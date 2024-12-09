#include "BlackHole.hpp" 

BlackHole::BlackHole() : gen(rd()), randomVelocity(1.0f, 5.0f) {
	mSprite.vertexData.Size = glm::vec2(400.0f, 400.0f);
	AABBVelocityMultiplier = 360.0f;

	mSprite.vertexData.Position = (EpicenterAABBPos + EpicenterAABBSize / 2.0f) - mSprite.vertexData.Size / 2.0f;
}

BlackHole::~BlackHole() {

}

void BlackHole::Update(std::vector<GameObject>& blocks, Actor& actor, const float& deltaTime, Animation& birthAnim, Animation& loopAnim, Mix_Chunk* bornSound, Mix_Chunk* consumedSound, Mix_Chunk* blackHoleIdle) {
	if (!birthTimerOneShot && !isBorn) {
		mSprite.vertexData.TexturePosition = loopAnim.TexturePosition;
		AnimationSize = loopAnim.Size;
		birthAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
		birthAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
		Mix_PlayChannel(8, bornSound, 0);
		birthTimerOneShot = true;
	}

	if (!isBorn) {
		if (!(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - birthAnim.AnimationTimer).count() > birthAnim.AnimationTime + deltaTime * 1000)) {
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - birthAnim.SingleFrameTimer).count() > birthAnim.SingleFrameTime + deltaTime * 1000) {
				birthAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
				birthAnim.index++;
			}
			mSprite.vertexData.TextureIndex = birthAnim.AnimationTextureIndexes[birthAnim.index];
		}
		else {
			birthAnim.index = 0;
			isBorn = true;
		}
		if (birthAnim.index > birthAnim.AnimationTextureIndexes.size() - 1) {
			birthAnim.index = 0;
		}
	}

	if (isBorn) {
		if (!loopTimerOneShot) {
			loopAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
			loopAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			loopTimerOneShot = true;
		}
		mSprite.vertexData.TextureIndex = loopAnim.AnimationTextureIndexes[loopAnim.index];
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - loopAnim.AnimationTimer).count() > loopAnim.AnimationTime + deltaTime * 1000) {
			loopAnim.AnimationTimer = std::chrono::high_resolution_clock::now();
			loopAnim.index = 0;
		}
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - loopAnim.SingleFrameTimer).count() > loopAnim.SingleFrameTime + deltaTime * 1000) {
			loopAnim.SingleFrameTimer = std::chrono::high_resolution_clock::now();
			loopAnim.index++;
		}
		if (loopAnim.index > loopAnim.AnimationTextureIndexes.size() - 1) {
			loopAnim.index = 0;
		}
	}

	AABBSize.x += AABBVelocityMultiplier * deltaTime;
	if (RectVsRect(AABBPos, AABBSize, actor.mPosition, actor.mSprite.vertexData.Size) && actor.isSucked == false && actor.isConsumedByVoid == false && actor.isSuckedPortal == false && actor.mEscaped == false && actor.mDead == false) {
		actor.isCollidable = false;
		actor.isSucked = true;
		actor.flyDirectionNormalized = glm::normalize(EpicenterAABBPos - actor.mPosition);
		actor.velocity = actor.flyDirectionNormalized * (float)randomVelocity(gen);
	}
	if (actor.isSucked == true) {
		if (actor.mPosition.x <= EpicenterAABBPos.x + EpicenterAABBSize.x / 2) {
			actor.isConsumedByVoid = true;
			actor.isVisible = false;
			actor.isSucked = false;
			Mix_PlayChannel(11, consumedSound, 0);
			actor.velocity = glm::vec2(0.0f, 0.0f);
		}

		actor.velocity += actor.velocity * 2.16f * deltaTime;
		
		if (actor.flyAngleTarget == -1.0f) {
			float dot = glm::dot(actor.flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));
			actor.flyAngleTarget = glm::acos(dot);
		}
		glm::vec2 center = glm::vec2(actor.mSprite.vertexData.Position.x + actor.mSprite.vertexData.Size.x / 2, actor.mSprite.vertexData.Position.y + actor.mSprite.vertexData.Size.y / 2);

		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(center, 0.0f));

		float crossProduct = glm::cross(actor.flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

		if (crossProduct > 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, -actor.flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (crossProduct < 0) {
			actor.mModelMatrix = glm::rotate(actor.mModelMatrix, actor.flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		actor.mModelMatrix = glm::translate(actor.mModelMatrix, glm::vec3(-center, 0.0f));

		if (actor.flyAngle < actor.flyAngleTarget) {
			actor.flyAngle += 0.1f * deltaTime;
		}
	}

	//maybe create a vector of all sucked blocks
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i].isSucked == true && blocks[i].isConsumedByVoid == false) {
			blocks[i].isCollidable = false;
			blocks[i].mSprite.vertexData.Position = blocks[i].mSprite.vertexData.Position + blocks[i].tempVelocity * deltaTime;
			if (blocks[i].tempVelocity.x > -100000.0f) {
			blocks[i].tempVelocity += blocks[i].tempVelocity * 2.16f * deltaTime;
			}

			if (blocks[i].mSprite.vertexData.Position.x <= EpicenterAABBPos.x + EpicenterAABBSize.x / 2) {
				blocks[i].isConsumedByVoid = true;
				blocks[i].isVisible = false;
				blocks[i].isSucked = false;
				blocks[i].tempVelocity = glm::vec2(0.0f, 0.0f);
			}
		}
		if (blocks[i].isVisible == true && blocks[i].isSucked == true) {

			if (blocks[i].flyAngleTarget == -1.0f) {
				float dot = glm::dot(blocks[i].flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));
				blocks[i].flyAngleTarget = glm::acos(dot);
			}

			glm::mat4 model = glm::mat4(1.0f);

			glm::vec2 center = glm::vec2(blocks[i].mSprite.vertexData.Position.x + blocks[i].mSprite.vertexData.Size.x / 2, blocks[i].mSprite.vertexData.Position.y + blocks[i].mSprite.vertexData.Size.y / 2);

			model = glm::translate(model, glm::vec3(center, 0.0f));

			float crossProduct = glm::cross(blocks[i].flyDirectionNormalized, glm::vec2(-1.0f, 0.0f));

			if (crossProduct > 0) {
				model = glm::rotate(model, -blocks[i].flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (crossProduct < 0) {
				model = glm::rotate(model, blocks[i].flyAngle, glm::vec3(0.0f, 0.0f, 1.0f));
			}
			blocks[i].mModelMatrix = glm::translate(model, glm::vec3(-center, 0.0f));

			if (blocks[i].flyAngle < blocks[i].flyAngleTarget) {
				blocks[i].flyAngle += 1.0f * deltaTime;
			}
		}
		if ((blocks[i].mSprite.vertexData.Position.x > AABBPos.x - AABBSize.x * 2) && (blocks[i].mSprite.vertexData.Position.x < AABBPos.x + AABBSize.x * 2)) {
			if (RectVsRect(AABBPos, AABBSize, blocks[i].mSprite.vertexData.Position, blocks[i].mSprite.vertexData.Size) && blocks[i].isVisible == true && blocks[i].isSucked == false) {
				affectedBlocks.push_back({ i , glm::distance2(EpicenterAABBPos, blocks[i].mSprite.vertexData.Position) });
			}
		}
		std::sort(affectedBlocks.begin(), affectedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
			return a.second < b.second;
			});

		for (int i = 0; i < affectedBlocks.size(); i++) {
			blocks[affectedBlocks[i].first].isVisible = true;
			blocks[affectedBlocks[i].first].isCollidable = false;
			blocks[affectedBlocks[i].first].isSucked = true;
			blocks[affectedBlocks[i].first].flyDirectionNormalized = glm::normalize(EpicenterAABBPos - blocks[affectedBlocks[i].first].mSprite.vertexData.Position);
			blocks[affectedBlocks[i].first].tempVelocity = blocks[affectedBlocks[i].first].flyDirectionNormalized * (float)randomVelocity(gen);
		}
		affectedBlocks.clear();
	}


	if (isBorn) {
		float distance = glm::distance2(mSprite.vertexData.Position + mSprite.vertexData.Size / 2.0f, actor.mPosition + actor.mSprite.vertexData.Size / 2.0f);

		if (distance < idleRadius) {
			idleVolume = 128.0f - (distance / idleRadius * 128.0f);
			Mix_Volume(13, idleVolume);
			if (!Mix_Playing(13)) {
				Mix_PlayChannel(13, blackHoleIdle, 0);
			}
		}
		else {
			idleVolume = 0;
			Mix_Volume(13, idleVolume);
		}
	}
}