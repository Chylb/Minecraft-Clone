#pragma once

#include "Block.h"

class Log : public Block
{
public:
	inline static const auto& axis = BlockStateProperties::axis;
	
	Log();

	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
};