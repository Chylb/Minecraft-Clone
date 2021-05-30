#include "Log.h"

Log::Log() : Block(true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::axis))
{
}