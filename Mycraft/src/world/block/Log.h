#pragma once

#include "Block.h"
#include "../../state/properties/BlockStateProperties.h"

class Log : public Block
{
public:
	Log(GLuint topTex, GLuint sideTex);
};