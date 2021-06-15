#pragma once

#include <glm/glm.hpp>

#include "../../world/block/BlockPos.h"

class World;

struct BlockRayTraceResult
{
	glm::vec3 location;
	bool hit;
	BlockPos blockPos;
	Direction direction;
};
