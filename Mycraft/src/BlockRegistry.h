#pragma once

#include <vector>

#include "world/block/Block.h"

class BlockRegistry
{
public:
	static Block* Register(Block* block);
	static Block* GetBlock(uint16_t id);

private:
	inline static std::vector<Block*> s_blocks;
};