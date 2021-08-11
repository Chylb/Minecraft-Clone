#pragma once

#include <vector>

#include "world/block/Block.h"
#include "world/block/BlockState.h"

class BlockRegistry
{
public:
	static Block* Register(Block* block);
	static BlockState* GetBlockState(uint16_t id);
	static int GetBlockStateCount();
	static const std::vector<Block*>& GetBlocks();

private:
	inline static std::vector<Block*> s_blocks;
	inline static std::vector<BlockState*> s_blockStates;
};