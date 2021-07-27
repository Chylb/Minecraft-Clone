#pragma once

#include "Block.h"

class DebugBlock : public Block
{
public:
	inline static const VoxelShape shape = Box(4, 4, 4, 12, 12, 12);

	DebugBlock();

	const VoxelShape& GetShape(const BlockState& state) const override;
	bool OccludesFace(Direction dir, const BlockState& state) const override;
	bool Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const override;
};