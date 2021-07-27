#pragma once

#include "Block.h"

class Dirt : public Block
{
public:
	Dirt();

	void RandomTick(const BlockState& state, World& world, BlockPos pos) const override;
};