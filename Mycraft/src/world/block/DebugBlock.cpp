#include "DebugBlock.h"

#include "../World.h"

DebugBlock::DebugBlock()
{
}

const VoxelShape& DebugBlock::GetShape(const BlockState& state) const
{
	return shape;
}

bool DebugBlock::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

bool DebugBlock::Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const
{
	printf("debug\n");
	for (Direction dir : Direction::directions)
	{
		auto& state = *world.GetBlockState(pos.Adjacent(dir));
		printf("%s %d %s \n", dir.ToString().c_str(), state.GetId(), typeid(state.GetBlock()).name());
	}
	printf("\n");
	return true;
}
