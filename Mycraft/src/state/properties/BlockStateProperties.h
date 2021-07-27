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

enum class DoorHingeSide
{
	left, right
};

enum class DoubleBlockHalf
{
	upper, lower
};

enum class RedstoneSide
{
	up, side, none
};

class BlockStateProperties
{
public:

	inline const static Property<Direction::Axis> axis = { {Direction::Axis::x, Direction::Axis::y, Direction::Axis::z} };
	inline const static Property<Direction::Value> horizontalFacing = { {Direction::north, Direction::east, Direction::south, Direction::west} };
	inline const static Property<SlabType> slabType = { {SlabType::top, SlabType::bottom, SlabType::full} };
	inline const static Property<Half> half = { {Half::top, Half::bottom} };
	inline const static Property<DoorHingeSide> doorHinge = { { DoorHingeSide::left, DoorHingeSide::right } };
	inline const static Property<DoubleBlockHalf> doubleBlockHalf = { { DoubleBlockHalf::upper, DoubleBlockHalf::lower } };
	inline const static Property<bool> open = { {false,true} };
	inline const static Property<bool> powered = { {false,true} };
	inline const static Property<bool> lit = { {false,true} };
	inline const static Property<bool> poweredUp = { {false,true} };
	inline const static Property<bool> poweredDown = { {false,true} };
	inline const static Property<bool> poweredNorth = { {false,true} };
	inline const static Property<bool> poweredEast = { {false,true} };
	inline const static Property<bool> poweredSouth = { {false,true} };
	inline const static Property<bool> poweredWest = { {false,true} };
	inline const static Property<RedstoneSide> eastRedstone = { {RedstoneSide::up, RedstoneSide::side, RedstoneSide::none} };
	inline const static Property<RedstoneSide> northRedstone = { {RedstoneSide::up, RedstoneSide::side, RedstoneSide::none} };
	inline const static Property<RedstoneSide> southRedstone = { {RedstoneSide::up, RedstoneSide::side, RedstoneSide::none} };
	inline const static Property<RedstoneSide> westRedstone = { {RedstoneSide::up, RedstoneSide::side, RedstoneSide::none} };
	inline const static Property<int> power = { {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15} };
};