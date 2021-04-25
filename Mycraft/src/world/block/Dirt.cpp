#include "Dirt.h"

#include "../World.h"

Dirt::Dirt(GLuint tex) : Block(tex, true)
{
}

void Dirt::Tick(World& world, BlockPos pos)
{
	auto blockAbove = world.GetBlock(pos.Adjacent<Direction::top>());
	if (blockAbove->GetId() == 0)
		world.SetBlock(pos, Blocks::grass->GetId());
}
