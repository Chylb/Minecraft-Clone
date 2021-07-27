#pragma once

#include <string>

#include "../../utils/Direction.h"

struct BlockPos
{
	int x;
	int y;
	int z;

	constexpr BlockPos Adjacent(Direction dir) const
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
	constexpr BlockPos Adjacent() const
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

	constexpr BlockPos Below()
	{
		return Adjacent<Direction::down>();
	}

	constexpr BlockPos Above()
	{
		return Adjacent<Direction::up>();
	}

	constexpr BlockPos North()
	{
		return Adjacent<Direction::north>();
	}

	constexpr BlockPos East()
	{
		return Adjacent<Direction::east>();
	}

	constexpr BlockPos South()
	{
		return Adjacent<Direction::south>();
	}

	constexpr BlockPos West()
	{
		return Adjacent<Direction::west>();
	}

	BlockPos& Set(BlockPos pos)
	{
		this->x = pos.x;
		this->y = pos.y;
		this->z = pos.z;
		return *this;
	}

	BlockPos& SetWithOffSet(BlockPos pos, Direction dir)
	{
		auto dirVec = dir.GetVector();
		return Set({ pos.x + (int)dirVec.x, pos.y + (int)dirVec.y, pos.z + (int)dirVec.z });
	}

	BlockPos& Move(Direction dir)
	{
		auto dirVec = dir.GetVector();
		return Set({ x + (int)dirVec.x, y + (int)dirVec.y, z + (int)dirVec.z });
	}

	bool operator==(const BlockPos& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	std::string ToString()
	{
		return "{" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "}";
	}
};