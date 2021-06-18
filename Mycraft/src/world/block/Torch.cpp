#include "Torch.h"

#include "../World.h"

Torch::Torch() : Block()
{
}

bool Torch::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

bool Torch::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	auto posBelow = pos.Adjacent(Direction::down);
	auto belowState = world.GetBlockState(posBelow);
	return belowState->IsFaceSturdy(world, posBelow, Direction::up);
}

const VoxelShape& Torch::GetShape(const BlockState& state) const
{
	return AABB;
}


