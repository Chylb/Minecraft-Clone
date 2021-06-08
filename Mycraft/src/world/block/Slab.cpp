#include "Slab.h"

#include "../World.h"
#include "../../state/properties/BlockStateProperties.h"

Slab::Slab() : Block(true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::slabType))
{
}

const VoxelShape& Slab::GetShape(const BlockState& state) const
{
	if (state.GetValue(BlockStateProperties::slabType) == SlabType::full)
		return VoxelShapes::Block();
	if (state.GetValue(BlockStateProperties::slabType) == SlabType::bottom)
		return bottomAABB;

	return topAABB;
}

const BlockState* Slab::GetStateForPlacement(BlockRayTraceResult rayTraceResult) const
{
	const auto& block = rayTraceResult.world->GetBlockState(rayTraceResult.blockPos)->GetBlock();
	if (&block == this)
		return &m_defaultBlockState->SetValue(BlockStateProperties::slabType, SlabType::full);

	if (rayTraceResult.location.y - rayTraceResult.blockPos.y > 0.5f)
		return &m_defaultBlockState->SetValue(BlockStateProperties::slabType, SlabType::top);

	return &m_defaultBlockState->SetValue(BlockStateProperties::slabType, SlabType::bottom);
}

bool Slab::CanBeReplaced(const BlockState& state, BlockRayTraceResult rayTraceResult) const
{
	const auto& block = rayTraceResult.world->GetBlockState(rayTraceResult.blockPos)->GetBlock();
	if (&block == this)
		return (state.GetValue(BlockStateProperties::slabType) != SlabType::full);

	return false;
}

bool Slab::OccludesFace(Direction::Direction dir, const BlockState& state) const
{
	if (state.GetValue(BlockStateProperties::slabType) == SlabType::full)
		return true;
	if (state.GetValue(BlockStateProperties::slabType) == SlabType::top && dir == Direction::top)
		return true;
	if (state.GetValue(BlockStateProperties::slabType) == SlabType::bottom && dir == Direction::bottom)
		return true;
	return false;
}
