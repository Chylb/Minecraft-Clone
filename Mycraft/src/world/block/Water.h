#pragma once

#include "Block.h"

class Water : public Block
{
public:
	Water();

	bool OccludesFace(Direction dir, const BlockState& state) const override;
	RenderType GetRenderType(const BlockState& state) const override;
};
