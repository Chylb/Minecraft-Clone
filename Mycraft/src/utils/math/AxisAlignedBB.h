#pragma once

#include "glm/vec3.hpp"

#include "BlockRayTraceResult.h"

struct AxisAlignedBB
{
	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;

	std::pair<Direction, float> GetHitDirection(glm::vec3 from, glm::vec3 rayDir, BlockPos pos) const;

	friend std::ostream& operator<<(std::ostream& os, const AxisAlignedBB& aabb);
};
