#pragma once

#include "../../utils/Direction.h"

struct BlockPos
{
	int x;
	int y;
	int z;
	
	constexpr BlockPos Adjacent(Direction dir)
	{
		switch (dir) {
		case Direction::north:
			return { x,y,z - 1 };

		case Direction::east:
			return { x + 1,y,z };

		case Direction::south:
			return { x,y,z + 1 };

		case Direction::west:
			return { x - 1,y,z };

		case Direction::up:
			return { x,y + 1,z };

		case Direction::down:
			return { x,y - 1,z };
		}
	}

	template <Direction dir>
	constexpr BlockPos Adjacent()
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
		if constexpr (dir == Direction::up) {
			return { x,y + 1,z };
		}
		if constexpr (dir == Direction::down) {
			return { x,y - 1,z };
		}
	}
};