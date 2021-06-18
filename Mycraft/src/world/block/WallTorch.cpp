#include "WallTorch.h"

#include "../World.h"
#include "../../item/BlockItemUseContext.h"

WallTorch::WallTorch()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this).AddProperty(facing));
	RegisterDefaultState(m_stateDefinition.Any()->SetValue(facing, Direction::north));
}

const VoxelShape& WallTorch::GetShape(const BlockState& state) const
{
	switch (state.GetValue(facing))
	{
	case Direction::north:
		return northAABB;
	case Direction::east:
		return eastAABB;
	case Direction::south:
		return southAABB;
	case Direction::west:
		return westAABB;
	}
}

const BlockState* WallTorch::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	if (useContext.GetClickedFace() == Direction::up) //todo uncanonical
		return Blocks::torch->GetStateForPlacement(useContext);

	auto state = m_defaultBlockState;
	auto& world = useContext.GetWorld();
	auto pos = useContext.GetClickedPos();

	for (auto dir : useContext.GetNearestLookingDirections())
	{
		if (dir.IsHorizontal())
		{
			state = &state->SetValue(facing, dir.GetOpposite()._value);
			if (state->CanSurvive(world, pos))
				return state;
		}
	}

	return nullptr;
}

bool WallTorch::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	Direction facingDir = state.GetValue(facing);
	auto neighborPos = pos.Adjacent(facingDir.GetOpposite());
	auto neighborState = world.GetBlockState(neighborPos);
	return neighborState->IsFaceSturdy(world, neighborPos, facingDir);
}
