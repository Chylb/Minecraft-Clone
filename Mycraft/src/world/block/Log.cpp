#include "Log.h"

Log::Log(GLuint topTex, GLuint sideTex) : Block(topTex, sideTex, topTex, true,
	StateContainer<Block, BlockState>::Builder(this).AddProperty(BlockStateProperties::axis))
{
}