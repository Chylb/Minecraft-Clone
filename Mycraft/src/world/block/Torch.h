#pragma once

#include "Block.h"

class Torch : public Block
{
public:
	inline static const VoxelShape AABB = Box(6, 0, 6, 10, 10, 10);

	Torch();

	bool OccludesFace(Direction dir, const BlockState& state) const override;
	bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const override;
	const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const override;
	const VoxelShape& GetShape(const BlockState& state) const override;
};
