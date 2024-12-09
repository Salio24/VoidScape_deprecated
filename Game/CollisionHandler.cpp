#define GLM_ENABLE_EXPERIMENTAL
#include "CollisionHandler.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Actor.hpp"


bool PointVsRect(const glm::vec2& p, const Box* r)
{
	return (p.x >= r->Position.x && p.y >= r->Position.y && p.x < r->Position.x + r->Size.x && p.y < r->Position.y + r->Size.y);
}

bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size)
{
	return (rect1Pos.x < rect2Pos.x + rect2Size.x && rect1Pos.x + rect1Size.x > rect2Pos.x && rect1Pos.y < rect2Pos.y + rect2Size.y && rect1Pos.y + rect1Size.y > rect2Pos.y);
}

bool RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear)
{
	contactNormal = { 0,0 };
	contactPoint = { 0,0 };

	// Cache division
	glm::vec2 invDirection = 1.0f / rayDirection;

	// Calculate intersections with rectangle bounding axes
	glm::vec2 timeNear = (target->Position - rayOrigin) * invDirection;
	glm::vec2 timeFar = (target->Position + target->Size - rayOrigin) * invDirection;

	if (std::isnan(timeFar.y) || std::isnan(timeFar.x)) {
		return false;
	}
	if (std::isnan(timeNear.y) || std::isnan(timeNear.x)) {
		return false;
	}

	// Sort distances
	if (timeNear.x > timeFar.x) {
		std::swap(timeNear.x, timeFar.x);
	}
	if (timeNear.y > timeFar.y) {
		std::swap(timeNear.y, timeFar.y);
	}

	// Early rejection		
	if (timeNear.x > timeFar.y || timeNear.y > timeFar.x) {
		return false;
	}

	// Closest 'time' will be the first contact
	hitTimeNear = std::max(timeNear.x, timeNear.y);

	// Furthest 'time' is contact on opPositionite side of target
	float hitTimeFar = std::min(timeFar.x, timeFar.y);

	// Reject if ray direction is pointing away from object
	if (hitTimeFar < 0) {
		return false;
	}

	// Contact point of collision from parametric line equation
	contactPoint = rayOrigin + hitTimeNear * rayDirection;

	if (timeNear.x > timeNear.y) {
		if (invDirection.x < 0) {
			contactNormal = { 1, 0 };
		}
		else {
			contactNormal = { -1, 0 };
		}
	}
	else if (timeNear.x < timeNear.y) {
		if (invDirection.y < 0) {
			contactNormal = { 0, 1 };
		}
		else {
			contactNormal = { 0, -1 };
		}
	}
	return true;
}

bool DynamicRectVsRect(const Box& dynamicBox, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity,
	glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position)
{
	if (dynamicBoxVelocity.x == 0 && dynamicBoxVelocity.y == 0)
		return false;

	Box expanded_target;
	expanded_target.Position = staticBox.Position - ( dynamicBox.Size / 2.0f );
	expanded_target.Size = staticBox.Size + dynamicBox.Size;
	
	if (RayVsRect(position + (dynamicBox.Size / 2.0f), dynamicBoxVelocity * deltaTime, &expanded_target, contactPoint, contactNormal, contactTime)) {
		return (contactTime >= 0.0f && contactTime < 1.0f);
	}
	else {
		return false;
	}
}

bool ResolveDynamicRectVsRect(Box& dynamicBox, const float deltaTime, const Box& staticBox, glm::vec2& dynamicBoxVelocity, Actor& actor, glm::vec2& averagedNormal, bool& NormalGroundCheck)
{

	glm::vec2 contactPoint, contactNormal;
	float contactTime = 0.0f;
	if (DynamicRectVsRect(actor.mSprite.vertexData, deltaTime, staticBox, actor.velocity, contactPoint, contactNormal, contactTime, actor.mPosition))
	{
		//if (contactNormal.y > 0) dynamicBox->contact[0] = staticBox; else nullptr;
		//if (contactNormal.x < 0) dynamicBox->contact[1] = staticBox; else nullptr;
		//if (contactNormal.y < 0) dynamicBox->contact[2] = staticBox; else nullptr;
		//if (contactNormal.x > 0) dynamicBox->contact[3] = staticBox; else nullptr;

		if (contactNormal.x >= 1 && contactNormal.x != 0 && actor.velocity.y != 0) {
			actor.isWallMountableL = true; 
		}
		else {
			actor.isWallMountableL = false;
		} 
		if (contactNormal.x <= -1 && contactNormal.x != 0 && actor.velocity.y != 0) {
			actor.isWallMountableR = true; 
		} 
		else {
			actor.isWallMountableR = false;
		}

		averagedNormal = glm::mix(averagedNormal, contactNormal, 0.5f);

		if (averagedNormal.y > 0.25f) {
			NormalGroundCheck = true;
		}
		else {
			NormalGroundCheck = false;
		}

		actor.velocity += contactNormal * glm::vec2(std::abs(actor.velocity.x), std::abs(actor.velocity.y)) * (1 - contactTime);
		if (contactNormal.x < 0) {
		
		}
		return true;
	}
	return false;
}

void CollisionUpdate(const std::vector<GameObject>& blocks, Actor& actor, bool& LeftWallHug, bool& RightWallHug, const float& deltaTime, bool& isGrounded) {

	static glm::vec2 averagedNormal(0.0f, 0.0f);
	static bool NormalGroundCheck = false;
	static bool BottomWallHug = false;
	
	glm::vec2 contactPoint, contactNormal;

	float contactTime = 0;

	std::vector<std::pair<int, float>> colidedBlocks;

	// Bottom left corner of broad-phase-box
	glm::vec2 A(actor.mPosition.x - 3 * actor.mSprite.vertexData.Size.x, actor.mPosition.y - 3 * actor.mSprite.vertexData.Size.y);
	// Top right corner of broad-phase-box
	glm::vec2 B(actor.mPosition.x + 4 * actor.mSprite.vertexData.Size.x, actor.mPosition.y + 4 * actor.mSprite.vertexData.Size.y);

	LeftWallHug = false;
	RightWallHug = false;
	BottomWallHug = false;
	for (int i = 0; i < blocks.size(); i++) {
		if (blocks[i].mSprite.vertexData.Position.x > A.x && blocks[i].mSprite.vertexData.Position.x < B.x && blocks[i].mSprite.vertexData.Position.y > A.y && blocks[i].mSprite.vertexData.Position.y < B.y && actor.isCollidable == true) {
			if (!blocks[i].isDeathTrigger && blocks[i].isCollidable == true) {
				if (DynamicRectVsRect(actor.mSprite.vertexData, deltaTime, blocks[i].mSprite.vertexData, actor.velocity, contactPoint, contactNormal, contactTime, actor.mPosition)) {
					colidedBlocks.push_back({ i, contactTime });
				}
				if (blocks[i].mSprite.vertexData.Position.x == actor.mPosition.x + actor.mSprite.vertexData.Size.x && actor.mPosition.y < blocks[i].mSprite.vertexData.Position.y + blocks[i].mSprite.vertexData.Size.y && actor.mPosition.y + actor.mSprite.vertexData.Size.y > blocks[i].mSprite.vertexData.Position.y && !isGrounded) {
					RightWallHug = RectVsRect(glm::vec2(actor.mPosition.x + actor.mSprite.vertexData.Size.x / 2, actor.mPosition.y), actor.mSprite.vertexData.Size, blocks[i].mSprite.vertexData.Position, blocks[i].mSprite.vertexData.Size);
				}
				if (blocks[i].mSprite.vertexData.Position.x + blocks[i].mSprite.vertexData.Size.x == actor.mPosition.x && actor.mPosition.y < blocks[i].mSprite.vertexData.Position.y + blocks[i].mSprite.vertexData.Size.y && actor.mPosition.y + actor.mSprite.vertexData.Size.y > blocks[i].mSprite.vertexData.Position.y && !isGrounded) {
					LeftWallHug = RectVsRect(glm::vec2(actor.mPosition.x - actor.mSprite.vertexData.Size.x / 2, actor.mPosition.y), actor.mSprite.vertexData.Size, blocks[i].mSprite.vertexData.Position, blocks[i].mSprite.vertexData.Size);
				} 
				if (blocks[i].mSprite.vertexData.Position.y + blocks[i].mSprite.vertexData.Size.y == actor.mPosition.y && actor.mPosition.x < blocks[i].mSprite.vertexData.Position.x + blocks[i].mSprite.vertexData.Size.x && actor.mPosition.x + actor.mSprite.vertexData.Size.x > blocks[i].mSprite.vertexData.Position.x) {
					BottomWallHug = RectVsRect(glm::vec2(actor.mPosition.x, actor.mPosition.y - actor.mSprite.vertexData.Size.y / 2), actor.mSprite.vertexData.Size, blocks[i].mSprite.vertexData.Position, blocks[i].mSprite.vertexData.Size);
				}
			}
			else if (blocks[i].isDeathTrigger) {
				Box AABB;
				AABB.Position = blocks[i].mTriggerAABBPos;
				AABB.Size = blocks[i].mTriggerAABBSize;
				if (DynamicRectVsRect(actor.mSprite.vertexData, deltaTime, AABB, actor.velocity, contactPoint, contactNormal, contactTime, actor.mPosition)) {
					if (blocks[i].isCollidable) {
						colidedBlocks.push_back({ i, contactTime });
					}
					actor.mDead = true;
				}
				if (blocks[i].mSprite.vertexData.Position.y + blocks[i].mSprite.vertexData.Size.y == actor.mPosition.y && actor.mPosition.x < blocks[i].mSprite.vertexData.Position.x + blocks[i].mSprite.vertexData.Size.x && actor.mPosition.x + actor.mSprite.vertexData.Size.x > blocks[i].mSprite.vertexData.Position.x) {
					if (blocks[i].isCollidable) {
						BottomWallHug = RectVsRect(glm::vec2(actor.mPosition.x, actor.mPosition.y - actor.mSprite.vertexData.Size.y / 2), actor.mSprite.vertexData.Size, blocks[i].mSprite.vertexData.Position, blocks[i].mSprite.vertexData.Size);
					}
				}
			}
		}
	}

	std::sort(colidedBlocks.begin(), colidedBlocks.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
		return a.second < b.second;
		});

	for (auto j : colidedBlocks) {
		ResolveDynamicRectVsRect(actor.mSprite.vertexData, deltaTime, blocks[j.first].mSprite.vertexData, actor.velocity, actor, averagedNormal, NormalGroundCheck);
	}

	if (BottomWallHug && NormalGroundCheck && actor.velocity.y == 0.0f) {
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}
	actor.mPosition += actor.velocity * deltaTime;

	actor.mPosition = glm::vec2(float(std::round(actor.mPosition.x * 1000)) / 1000.0f, float(std::round(actor.mPosition.y * 1000)) / 1000.0f);
}