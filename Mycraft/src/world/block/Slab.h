#pragma once

#include "Block.h"

class Slab : public Block
{
public:
	inline static const auto& type = BlockStateProperties::slabType;
	
	Slab();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const override;
	bool OccludesFace(Direction dir, const BlockState& state) const override;
private:

	inline static const VoxelShape bottomAABB{ { 0, 0, 0, 1, 0.5, 1 } };
	inline static const VoxelShape topAABB{ {0, 0.5, 0, 1, 1, 1} };
};

