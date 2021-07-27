#pragma once

#include "RedstoneTorch.h"

class RedstoneWallTorch : public RedstoneTorch
{
public:
	inline static const auto& facing = BlockStateProperties::horizontalFacing;

	RedstoneWallTorch();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const override;
	bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const override;
	int GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const override;

protected:
	bool HasNeighborSignal(const World& world, BlockPos pos, const BlockState& state) const override;
};