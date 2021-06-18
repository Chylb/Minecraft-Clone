#pragma once

#include "Torch.h"

class WallTorch : public Torch
{
public:
	inline static const auto& facing = BlockStateProperties::horizontalFacing;

	inline static const VoxelShape northAABB = Box(5.5, 3.0, 11.0, 10.5, 13.0, 16.0);
	inline static const VoxelShape southAABB = Box(5.5, 3.0, 0.0, 10.5, 13.0, 5.0);
	inline static const VoxelShape westAABB = Box(11.0, 3.0, 5.5, 16.0, 13.0, 10.5);
	inline static const VoxelShape eastAABB = Box(0.0, 3.0, 5.5, 5.0, 13.0, 10.5);

	WallTorch();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const override;
};
