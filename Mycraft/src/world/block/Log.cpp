#include "Log.h"

Log::Log() : Block(true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::axis))
{
}

const BlockState* Log::GetStateForPlacement(BlockRayTraceResult rayTraceResult) const
{
	return &m_defaultBlockState->SetValue(BlockStateProperties::axis, Direction::GetAxis(rayTraceResult.direction));
}
