#pragma once

#include "../Property.h"

enum class Axis
{
	x, y, z
};

class BlockStateProperties
{
public:
	static void Initialize();

	inline static Property<Axis>* axis;
};