#pragma once

struct ChunkPos
{
	int x;
	int z;

	constexpr ChunkPos Adjacent(Direction dir)
	{
		if (dir == Direction::north) {
			return { x,z - 1 };
		}
		if (dir == Direction::east) {
			return { x + 1,z };
		}
		if (dir == Direction::south) {
			return { x,z + 1 };
		}
		if (dir == Direction::west) {
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