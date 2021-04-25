#pragma once

#include "Block.h"

class Dirt : public Block
{
public:
	Dirt(GLuint tex);

	void Tick(World& world, BlockPos pos);
};