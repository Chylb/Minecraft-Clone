#pragma once

#include "Block.h"

class Flower : public Block
{
public:
	Flower();

	const VoxelShape& GetShape(const BlockState& state) const override;
	bool OccludesFace(Direction dir, const BlockState& state) const override;
private:
	inline static const VoxelShape shape = Box(5, 0, 5, 11, 10, 11);
};

