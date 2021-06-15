#pragma once

#include "../Property.h"
#include "../utils/Direction.h"

enum class SlabType
{
	top, bottom, full
};

enum class Half
{
	top, bottom
};

class BlockStateProperties
{
public:

	inline const static Property<Direction::Axis> axis = { {Direction::Axis::x, Direction::Axis::y, Direction::Axis::z} };
	inline const static Property<Direction::Value> horizontalFacing = { {Direction::north, Direction::east, Direction::south, Direction::west} };
	inline const static Property<SlabType> slabType = { {SlabType::top, SlabType::bottom, SlabType::full} };
	inline const static Property<Half> half = { {Half::top, Half::bottom} };
	inline const static Property<bool> open = { {false,true} };
};