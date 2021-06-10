#include "Dirt.h"

#include "../World.h"

Dirt::Dirt() : Block(true)
{
}

void Dirt::Tick(World& world, BlockPos pos) const
{
	auto stateAbove = world.GetBlockState(pos.Adjacent(Direction::up));
	if (stateAbove->GetId() == 0)
		world.SetBlock(pos, Blocks::grass->DefaultBlockState());
}
