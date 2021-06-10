#pragma once

#include "../Property.h"
#include "../utils/Direction.h"

enum class SlabType
{
	top, bottom, full
};

class BlockStateProperties
{
public:
	static void Initialize();

	inline static Property<Direction::Axis>* axis;
	inline static Property<SlabType>* slabType;
};