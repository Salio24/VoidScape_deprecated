#pragma once
#include "GameEntity.hpp"
#include <tuple>
#include <limits>
#include <algorithm>
#include <cmath>

bool PointVsRect(const glm::vec2& p, const Box* r);

bool RectVsRect(const glm::vec2 rect1Pos, const glm::vec2 rect1Size, const glm::vec2 rect2Pos, const glm::vec2 rect2Size);

bool RayVsRect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const Box* target, glm::vec2& contactPoint, glm::vec2& contactNormal, float& hitTimeNear);

bool DynamicRectVsRect(const Box& dynamicBox, const float deltaTime, const Box& staticBox, const glm::vec2& dynamicBoxVelocity, glm::vec2& contactPoint, glm::vec2& contactNormal, float& contactTime, glm::vec2& position);

bool ResolveDynamicRectVsRect(Box& dynamicBox, const float deltaTime, const Box& staticBox, glm::vec2& dynamicBoxVelocity, GameEntity& actor, glm::vec2& averagedNormal, bool& NormalGroundCheck);

void CollisionUpdate(const std::vector<GameObject>& blocks, GameEntity& actor, bool& LeftWallHug, bool& RightWallHug, const float& deltaTime);