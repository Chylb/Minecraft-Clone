#include "Log.h"

#include "../../item/BlockItemUseContext.h"

Log::Log() : Block(StateContainer<Block, BlockState>::Builder(this).AddProperty(axis))
{
}

const BlockState* Log::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	return &m_defaultBlockState->SetValue(axis, useContext.GetClickedFace().GetAxis());
}
