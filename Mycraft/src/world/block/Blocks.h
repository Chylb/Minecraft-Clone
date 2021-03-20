#pragma once

#include <unordered_map>

#include "Block.h"
#include "../../BlockRegistry.h"
#include "../../Resources.h"

class Blocks
{
public:
	static void Initialize();
	static Block* GetBlock(uint16_t id);

	inline static Block* air;
	inline static Block* stone;
	inline static Block* grass;
	inline static Block* dirt;
	inline static Block* cobblestone;
	inline static Block* plank;
	inline static Block* wood;
	inline static Block* leaves;
};
