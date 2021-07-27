#pragma once

#include "Block.h"

class Door : public Block
{
public:
	inline static const auto& facing = BlockStateProperties::horizontalFacing;
	inline static const auto& open = BlockStateProperties::open;
	inline static const auto& hinge = BlockStateProperties::doorHinge;
	inline static const auto& half = BlockStateProperties::doubleBlockHalf;
	//inline static const auto& powered = BlockStateProperties::powered;

	inline static const auto& southAABB = Box(0, 0, 0, 16, 16, 3);
	inline static const auto& northAABB = Box(0, 0, 13, 16, 16, 16);
	inline static const auto& westAABB = Box(13, 0, 0, 16, 16, 16);
	inline static const auto& eastAABB = Box(0, 0, 0, 3, 16, 16);

	Door();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const override;
	void SetPlacedBy(World& world, BlockPos pos, const BlockState& state) const override;
	bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const override;
	const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const override;
	void NeighborChanged(const BlockState& state, World& world, BlockPos pos, const Block& updaterBlock, BlockPos updaterPos) const override;

	bool OccludesFace(Direction dir, const BlockState& state) const override;

private:
	DoorHingeSide GetHinge(const BlockItemUseContext& useContext) const;
};
