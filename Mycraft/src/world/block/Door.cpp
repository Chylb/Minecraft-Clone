#include "Door.h"

#include "../World.h"
#include "../../item/BlockItemUseContext.h"

Door::Door()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this).AddProperty(half).AddProperty(facing).AddProperty(open).AddProperty(hinge));
	RegisterDefaultState(m_stateDefinition.Any()->SetValue(facing, Direction::north).SetValue(open, false).SetValue(hinge, DoorHingeSide::left).SetValue(half, DoubleBlockHalf::lower));
}

bool Door::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

const VoxelShape& Door::GetShape(const BlockState& state) const
{
	bool closed = !state.GetValue(open);
	bool right = state.GetValue(hinge) == DoorHingeSide::right;

	switch (state.GetValue(facing))
	{
	case Direction::east:
		return closed ? eastAABB : (right ? northAABB : southAABB);
	case Direction::south:
		return closed ? southAABB : (right ? eastAABB : westAABB);
	case Direction::west:
		return closed ? westAABB : (right ? southAABB : northAABB);
	case Direction::north:
		return closed ? northAABB : (right ? westAABB : eastAABB);
	}
}

const BlockState* Door::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	BlockPos pos = useContext.GetClickedPos();
	if (pos.y < 255 && useContext.GetWorld().GetBlockState(pos.Adjacent(Direction::up))->CanBeReplaced(useContext))
	{
		return &m_defaultBlockState->SetValue(facing, useContext.GetHorizontalDirection()._value).SetValue(hinge, GetHinge(useContext)).SetValue(half, DoubleBlockHalf::lower);
	}
	return nullptr;
}

bool Door::Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const
{
	auto newState = state.Cycle(open);
	world.SetBlock(pos, newState);
	return true;
}

void Door::SetPlacedBy(World& world, BlockPos pos, const BlockState& state) const
{
	world.SetBlock(pos.Adjacent(Direction::up), state.SetValue(half, DoubleBlockHalf::upper));
}

bool Door::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	auto belowPos = pos.Adjacent(Direction::down);
	auto belowState = world.GetBlockState(belowPos);
	return state.GetValue(half) == DoubleBlockHalf::lower ? belowState->IsFaceSturdy(world, belowPos, Direction::up) : belowState->Is(*this);
}

const BlockState& Door::UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	auto oldHalf = oldState.GetValue(half);
	if (from.GetAxis() == Direction::Axis::y && (oldHalf == DoubleBlockHalf::lower) == (from == Direction::up))
	{
		return updaterState.Is(*this) && updaterState.GetValue(half) != oldHalf ? oldState.SetValue(facing, updaterState.GetValue(facing)).SetValue(open, updaterState.GetValue(open)).SetValue(hinge, updaterState.GetValue(hinge)) : *Blocks::air->DefaultBlockState();
	}
	return oldHalf == DoubleBlockHalf::lower && from == Direction::down && !oldState.CanSurvive(world, pos) ? *Blocks::air->DefaultBlockState() : Block::UpdateShape(oldState, from, updaterState, world, pos);
}


DoorHingeSide Door::GetHinge(const BlockItemUseContext& useContext) const
{
	auto pos = useContext.GetClickedPos();
	Direction lookingDir = useContext.GetHorizontalDirection();
	int dirX = lookingDir.GetVector().x;
	int dirZ = lookingDir.GetVector().z;
	auto clickLocation = useContext.GetClickLocation();

	float dx = clickLocation.x - pos.x;
	float dz = clickLocation.z - pos.z;

	return (dirX >= 0 || !(dz < 0.5)) && (dirX <= 0 || !(dz > 0.5)) && (dirZ >= 0 || !(dx > 0.5)) && (dirZ <= 0 || !(dx < 0.5)) ? DoorHingeSide::left : DoorHingeSide::right;
}
