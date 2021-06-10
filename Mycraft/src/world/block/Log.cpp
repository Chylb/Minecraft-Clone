#include "Log.h"

//#include "../../Direction.cpp"

Log::Log() : Block(true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::axis))
{
}

const BlockState* Log::GetStateForPlacement(BlockRayTraceResult rayTraceResult) const
{
	return &m_defaultBlockState->SetValue(BlockStateProperties::axis, rayTraceResult.direction.GetAxis());
}
