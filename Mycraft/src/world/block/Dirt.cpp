#include "Dirt.h"

#include "../World.h"

Dirt::Dirt(GLuint tex) : Block(tex, true)
{
}

void Dirt::Tick(World& world, BlockPos pos)
{
	auto stateAbove = world.GetBlockState(pos.Adjacent<Direction::top>());
	if (stateAbove->GetId() == 0)
		world.SetBlock(pos, Blocks::grass->DefaultBlockState());
}
