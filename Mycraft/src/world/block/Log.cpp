#include "Log.h"

#include "../../item/BlockItemUseContext.h"

Log::Log() : Block(true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::axis))
{
}

const BlockState* Log::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	return &m_defaultBlockState->SetValue(BlockStateProperties::axis, useContext.GetClickedFace().GetAxis());
}
