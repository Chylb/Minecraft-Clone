#include "RedstoneWallTorch.h"

#include "Blocks.h"
#include "../World.h"

RedstoneWallTorch::RedstoneWallTorch()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this).AddProperty(lit).AddProperty(facing));
	RegisterDefaultState(m_stateDefinition.Any()->SetValue(facing, Direction::north).SetValue(lit, true));
}

const VoxelShape& RedstoneWallTorch::GetShape(const BlockState& state) const
{
	return Blocks::wallTorch->GetShape(state);
}

const BlockState* RedstoneWallTorch::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	auto state = Blocks::wallTorch->GetStateForPlacement(useContext);
	return state->HasProperty(facing) ? &m_defaultBlockState->SetValue(facing, state->GetValue(facing)) : Blocks::redstoneTorch->DefaultBlockState();
}

const BlockState& RedstoneWallTorch::UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	return Blocks::wallTorch->UpdateShape(oldState, from, updaterState, world, pos);
}

bool RedstoneWallTorch::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	return Blocks::wallTorch->CanSurvive(state, world, pos);
}

int RedstoneWallTorch::GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const
{
	return state.GetValue(lit) && state.GetValue(facing) != dir ? 15 : 0;
}

bool RedstoneWallTorch::HasNeighborSignal(const World& world, BlockPos pos, const BlockState& state) const
{
	Direction dir = state.GetValue(facing);
	dir = dir.GetOpposite();

	auto& conductor = *world.GetBlockState(pos.Adjacent(dir));

	return world.GetSignal(pos.Adjacent(dir), dir) > 0;
}
