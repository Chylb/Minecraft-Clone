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
	inline static Block* flower;
	inline static Block* debugBlock;
	inline static Block* trapDoor;
	inline static Block* torch;
	inline static Block* wallTorch;
	inline static Block* door;
	inline static Block* redstoneWire;
	inline static Block* redstoneTorch;
	inline static Block* redstoneWallTorch;
	inline static Block* powerSensor;
};
