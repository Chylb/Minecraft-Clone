#include "RedstoneTorch.h"

#include "../World.h"

RedstoneTorch::RedstoneTorch()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this).AddProperty(lit));
	RegisterDefaultState(m_stateDefinition.Any()->SetValue(lit, true));
}

void RedstoneTorch::OnPlace(const BlockState& state, World& world, BlockPos pos, const BlockState& oldState) const
{
	for (Direction dir : Direction::directions)
		world.UpdateNeighborsAt(pos.Adjacent(dir), *this);
}

void RedstoneTorch::OnRemove(const BlockState& state, World& world, BlockPos pos, const BlockState& newState) const
{
	for (Direction dir : Direction::directions)
		world.UpdateNeighborsAt(pos.Adjacent(dir), *this);
}

void RedstoneTorch::NeighborChanged(const BlockState& state, World& world, BlockPos pos, const Block& updaterBlock, BlockPos updaterPos) const
{
	if (state.GetValue(lit) == this->HasNeighborSignal(world, pos, state) && !world.GetBlockTicks().WillTickThisTick(pos, *this));
	{
		world.GetBlockTicks().ScheduleTick(pos, *this, 2);
	}
}

void RedstoneTorch::Tick(const BlockState& state, World& world, BlockPos pos) const
{
	bool hasNeighborSignal = HasNeighborSignal(world, pos, state);

	if (state.GetValue(lit))
	{
		if (hasNeighborSignal)
		{
			world.SetBlock(pos, state.SetValue(lit, false), 3);
		}
	}
	else if (!hasNeighborSignal)
	{
		world.SetBlock(pos, state.SetValue(lit, true), 3);
	}
}

int RedstoneTorch::GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const
{
	return state.GetValue(lit) && dir != Direction::up ? 15 : 0;
}

int RedstoneTorch::GetDirectSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const
{
	return dir == Direction::down ? state.GetSignal(world, pos, dir) : 0;
}

bool RedstoneTorch::IsSignalSource(const BlockState& state) const
{
	return true;
}

bool RedstoneTorch::HasNeighborSignal(const World& world, BlockPos pos, const BlockState& state) const
{
	return world.GetSignal(pos.Below(), Direction::down) > 0;
}
