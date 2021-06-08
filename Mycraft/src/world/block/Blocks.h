#pragma once

#include "../../BlockRegistry.h"
#include "../../Resources.h"

class Blocks
{
public:
	static void Initialize();
	static BlockState* GetBlockState(uint16_t id);

	inline static Block* air;
	inline static Block* stone;
	inline static Block* grass;
	inline static Block* dirt;
	inline static Block* cobblestone;
	inline static Block* plank;
	inline static Block* wood;
	inline static Block* leaves;
	inline static Block* slab;
};
