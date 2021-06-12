#pragma once

#include "Block.h"
#include "../../state/properties/BlockStateProperties.h"

class Log : public Block
{
public:
	Log();

	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
};