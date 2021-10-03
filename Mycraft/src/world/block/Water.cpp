#include "Water.h"

Water::Water() : Block()
{
}

bool Water::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

RenderType Water::GetRenderType(const BlockState& state) const
{
	return RenderType::translucent;
}

