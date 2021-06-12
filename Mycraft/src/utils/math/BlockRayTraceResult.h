#pragma once

#include <glm/glm.hpp>

#include "../../world/block/BlockPos.h"

class World;

struct BlockRayTraceResult
{
	//World* world;
	glm::vec3 location;
	bool hit;
	BlockPos blockPos;
	Direction direction;
};
