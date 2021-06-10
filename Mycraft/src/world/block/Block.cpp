#include "Block.h"

#include "../World.h"
#include "../../utils/math/shapes/VoxelShapes.h"

Block::Block(bool opaque, StateContainer<Block, BlockState>::Builder builder) :
	m_opaque(opaque)
{
	m_stateDefinition = builder.Create();
	m_defaultBlockState = m_stateDefinition.Any();
}

Block::Block(bool opaque) :
	Block::Block(opaque, StateContainer<Block, BlockState>::Builder(this))
{
}

bool Block::IsOpaque() const
{
	return m_opaque;
}

const StateContainer<Block, BlockState>& Block::GetStateDefinition() const
{
	return m_stateDefinition;
}

BlockState* Block::DefaultBlockState() const
{
	return m_defaultBlockState;
}

const BlockState* Block::GetStateForPlacement(BlockRayTraceResult rayTraceResult) const
{
	return m_defaultBlockState;
}

const VoxelShape& Block::GetShape(const BlockState& state) const
{
	return VoxelShapes::Block();
}

void Block::Tick(World& world, BlockPos pos) const
{
}

bool Block::CanBeReplaced(const BlockState& state, BlockRayTraceResult rayTraceResult) const
{
	return false;
}

bool Block::OccludesFace(Direction dir, const BlockState& state) const
{
	return true;
}