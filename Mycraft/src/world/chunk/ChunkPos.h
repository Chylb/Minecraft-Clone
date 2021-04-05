#pragma once

struct ChunkPos
{
	int x;
	int z;

	template<Direction::Direction dir> constexpr ChunkPos Adjacent()
	{
		if constexpr (dir == Direction::north) {
			return { x,z - 1 };
		}
		if constexpr (dir == Direction::east) {
			return { x + 1,z };
		}
		if constexpr (dir == Direction::south) {
			return { x,z + 1 };
		}
		if constexpr (dir == Direction::west) {
			return { x - 1,z };
		}
	}

	bool operator==(const ChunkPos& other) const
	{
		return x == other.x && z == other.z;
	}

	struct HashFunction
	{
		std::size_t operator()(const ChunkPos& k) const
		{
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.z);
		}
	};
};