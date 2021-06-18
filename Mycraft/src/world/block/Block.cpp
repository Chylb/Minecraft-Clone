#include "Block.h"

#include "../World.h"
#include "../../utils/math/shapes/VoxelShapes.h"

Block::Block(StateContainer<Block, BlockState>::Builder builder)
{
	m_stateDefinition = builder.Create();
	m_defaultBlockState = m_stateDefinition.Any();
}

Block::Block() :
	Block::Block(StateContainer<Block, BlockState>::Builder(this))
{
}

void Block::CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder builder)
{
	m_stateDefinition = builder.Create();
	m_defaultBlockState = m_stateDefinition.Any();
}

void Block::RegisterDefaultState(const BlockState& state)
{
	m_defaultBlockState = &state;
}

const StateContainer<Block, BlockState>& Block::GetStateDefinition() const
{
	return m_stateDefinition;
}

const BlockState* Block::DefaultBlockState() const
{
	return m_defaultBlockState;
}

const BlockState* Block::GetStateForPlacement(const BlockItemUseContext& useContext) const
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

bool Block::CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const
{
	return false;
}

bool Block::Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const
{
	return false;
}

bool Block::OccludesFace(Direction dir, const BlockState& state) const
{
	return true;
}

bool Block::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	return true;
}

VoxelShape Block::Box(float x0, float y0, float z0, float x1, float y1, float z1)
{
	return VoxelShape({ {x0 / 16.0f,y0 / 16.0f,z0 / 16.0f,x1 / 16.0f,y1 / 16.0f,z1 / 16.0f} });
}
