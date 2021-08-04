#pragma once

#include <functional>

#include "../../world/block/BlockPos.h"
#include "../../world/block/BlockState.h"

class Block;

class BlockColors
{
public:
	static void Initialize();
	static void Register(std::function<glm::vec3(const BlockState& state, BlockPos pos, int ix)> colorFunc, const Block* block);
};
