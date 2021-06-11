#include "Flower.h"

#include "../../utils/math/shapes/VoxelShapes.h"

Flower::Flower() : Block(false)
{
}

const VoxelShape& Flower::GetShape(const BlockState& state) const
{
	return shape;
}

bool Flower::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}
