#pragma once

#include "Block.h"

class PowerSensor : public Block
{
public:
	inline static const auto& poweredUp = BlockStateProperties::poweredUp;
	inline static const auto& poweredDown = BlockStateProperties::poweredDown;
	inline static const auto& poweredNorth = BlockStateProperties::poweredNorth;
	inline static const auto& poweredEast = BlockStateProperties::poweredEast;
	inline static const auto& poweredSouth = BlockStateProperties::poweredSouth;
	inline static const auto& poweredWest = BlockStateProperties::poweredWest;

	PowerSensor();

	const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const override;
	void SetPlacedBy(World& world, BlockPos pos, const BlockState& state) const override;
	
	bool OccludesFace(Direction dir, const BlockState& state) const override;
};