#pragma once

#define FOREACH_CARDINAL_DIRECTION(VAR, CODE) \
	{VAR = Direction::north; CODE} \
	{VAR = Direction::east; CODE} \
	{VAR = Direction::south; CODE} \
	{VAR = Direction::west; CODE} 

#define FOREACH_DIRECTION(VAR, CODE) \
	FOREACH_CARDINAL_DIRECTION(VAR, CODE) \
	{VAR = Direction::top; CODE} \
	{VAR = Direction::bottom; CODE} 

namespace Direction
{
	enum Direction
	{
		north,
		east,
		south,
		west,
		top,
		bottom
	};

	template<Direction dir> constexpr Direction Opposite()
	{
		switch (dir) {
		case north:
			return south;
		case east:
			return west;
		case south:
			return north;
		case west:
			return east;
		case top:
			return bottom;
		case bottom:
			return top;
		}
	}

	template<Direction dir> constexpr bool IsCardinal()
	{
		return !(dir == top || dir == bottom);
	}
}
