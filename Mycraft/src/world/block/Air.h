#pragma once

#include "Block.h"

class Air : public Block
{
public:
	Air();

	const VoxelShape& GetShape(const BlockState& state) const override;
	bool CanBeReplaced(const BlockState& state, BlockRayTraceResult rayTraceResult) const override;
	bool OccludesFace(Direction dir, const BlockState& state) const override;
};