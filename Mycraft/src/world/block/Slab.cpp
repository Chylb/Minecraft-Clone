#include "Slab.h"

#include "../World.h"
#include "../../item/BlockItemUseContext.h"
#include "../../utils/math/shapes/VoxelShapes.h"

Slab::Slab() : Block(StateContainer<Block, BlockState>::Builder(this).AddProperty(type))
{
}

const VoxelShape& Slab::GetShape(const BlockState& state) const
{
	if (state.GetValue(type) == SlabType::full)
		return VoxelShapes::Block();
	if (state.GetValue(type) == SlabType::bottom)
		return bottomAABB;

	return topAABB;
}

const BlockState* Slab::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	auto pos = useContext.GetClickedPos();
	const auto state = useContext.GetWorld().GetBlockState(pos);
	if (&state->GetBlock() == this) {
		return &(state->SetValue(type, SlabType::full));
	}
	else {
		auto dir = useContext.GetClickedFace();
		auto& top = m_defaultBlockState->SetValue(type, SlabType::top);
		auto& bottom = m_defaultBlockState->SetValue(type, SlabType::bottom);
		return dir != Direction::down && (dir == Direction::up || !(useContext.GetClickLocation().y - pos.y > 0.5)) ? &bottom : &top;
	}
}

bool Slab::CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const
{
	SlabType slabType = state.GetValue(type);
	if (slabType != SlabType::full && &useContext.GetPlacedBlock() == this)
	{
		if (useContext.ReplacingClickedOnBlock())
		{
			bool hitUpper = useContext.GetClickLocation().y - useContext.GetClickedPos().y > 0.5f;
			auto dir = useContext.GetClickedFace();
			if (slabType == SlabType::bottom)
				return dir == Direction::up || hitUpper && dir.IsHorizontal();
			else
				return dir == Direction::down || !hitUpper && dir.IsHorizontal();
		}
		return true;
	}
	return false;
}

bool Slab::OccludesFace(Direction dir, const BlockState& state) const
{
	if (state.GetValue(type) == SlabType::full)
		return true;
	if (state.GetValue(type) == SlabType::bottom && dir == Direction::down)
		return true;
	if (state.GetValue(type) == SlabType::top && dir == Direction::up)
		return true;

	return false;
}
