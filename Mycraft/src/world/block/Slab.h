#pragma once

#include "Block.h"

#include "../../utils/math/shapes/VoxelShapes.h"

class Slab : public Block
{
public:
	Slab();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const override;
	bool OccludesFace(Direction dir, const BlockState& state) const override;
private:

	inline static const VoxelShape bottomAABB{ { 0, 0, 0, 1, 0.5, 1 } };
	inline static const VoxelShape topAABB{ {0, 0.5, 0, 1, 1, 1} };
};

