#pragma once

#include "Block.h"

class Dirt : public Block
{
public:
	Dirt();

	void Tick(World& world, BlockPos pos) const override;
};