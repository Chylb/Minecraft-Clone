#pragma once

#include "../../Direction.h"

struct BlockPos
{
	int x;
	int y;
	int z;

	template<Direction::Direction dir> constexpr BlockPos Adjacent()
	{
		if constexpr (dir == Direction::north) {
			return { x,y,z - 1 };
		}
		if constexpr (dir == Direction::east) {
			return { x + 1,y,z };
		}
		if constexpr (dir == Direction::south) {
			return { x,y,z + 1 };
		}
		if constexpr (dir == Direction::west) {
			return { x - 1,y,z };
		}
		if constexpr (dir == Direction::top) {
			return { x + 1,y,z };
		}
		if constexpr (dir == Direction::bottom) {
			return { x,y - 1,z };
		}
	}
};