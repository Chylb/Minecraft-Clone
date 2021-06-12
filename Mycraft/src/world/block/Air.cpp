#include "Air.h"

#include "../../utils/math/shapes/VoxelShapes.h"

Air::Air() : Block(false)
{
}

const VoxelShape& Air::GetShape(const BlockState& state) const
{
	return VoxelShapes::Empty();
}

bool Air::CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const
{
	return true;
}

bool Air::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}
