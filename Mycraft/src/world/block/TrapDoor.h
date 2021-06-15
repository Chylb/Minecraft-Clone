#pragma once

#include "Block.h"

class TrapDoor : public Block
{
public:
	inline static const auto& facing = BlockStateProperties::horizontalFacing;
	inline static const auto& open = BlockStateProperties::open;
	inline static const auto& half = BlockStateProperties::half;

	inline static const VoxelShape eastOpenAABB = Box(0, 0, 0, 3, 16, 16);
	inline static const VoxelShape westOpenAABB = Box(13, 0, 0, 16, 16, 16);
	inline static const VoxelShape southOpenAABB = Box(0, 0, 0, 16, 16, 3);
	inline static const VoxelShape northOpenAABB = Box(0, 0, 13, 16, 16, 16);
	inline static const VoxelShape bottomAABB = Box(0, 0, 0, 16, 3, 16);
	inline static const VoxelShape topAABB = Box(0, 13, 0, 16, 16, 16);

	TrapDoor();

	bool OccludesFace(Direction dir, const BlockState& state) const override;

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const override;
};
