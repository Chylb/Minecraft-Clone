#include "TrapDoor.h"

#include "../World.h"
#include "../../item/BlockItemUseContext.h"

TrapDoor::TrapDoor() : Block(StateContainer<Block, BlockState>::Builder(this).AddProperty(facing).AddProperty(open).AddProperty(half))
{
	m_defaultBlockState = const_cast<BlockState*>(&m_stateDefinition.Any()->SetValue(facing, Direction::north).SetValue(open, false).SetValue(half, Half::bottom));
}

bool TrapDoor::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

const VoxelShape& TrapDoor::GetShape(const BlockState& state) const
{
	if (!state.GetValue(open))
		return state.GetValue(half) == Half::top ? topAABB : bottomAABB;

	switch (state.GetValue(facing))
	{
	case Direction::north:
		return northOpenAABB;
	case Direction::east:
		return eastOpenAABB;
	case Direction::south:
		return southOpenAABB;
	case Direction::west:
		return westOpenAABB;
	}
}

const BlockState* TrapDoor::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	auto clickedFace = useContext.GetClickedFace();

	if (!useContext.ReplacingClickedOnBlock() && clickedFace.IsHorizontal())
		return  &m_defaultBlockState->SetValue(facing, clickedFace._value).SetValue(half, useContext.GetClickLocation().y - useContext.GetClickedPos().y > 0.5 ? Half::top : Half::bottom);

	return &m_defaultBlockState->SetValue(facing, useContext.GetHorizontalDirection().GetOpposite()._value).SetValue(half, clickedFace == Direction::up ? Half::bottom : Half::top);
}

bool TrapDoor::Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const
{
	auto& newState = state.SetValue(open, !state.GetValue(open));
	world.SetBlock(pos, newState);

	return true;
}
