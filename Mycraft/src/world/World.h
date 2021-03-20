#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "chunk/Chunk.h"
#include "block/BlockPos.h"
#include "worldgen/WorldGenerator.h"

struct pair_hash
{
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

class World
{
public:
	World();

	void LoadChunk(int x, int z);
	Chunk* GetChunk(int x, int z);
	Chunk* GetChunkAt(BlockPos pos);

	Block* GetBlock(BlockPos pos);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void Render();

private:
	std::vector<Chunk> m_chunks;
	std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> m_chunkMap;
	std::unordered_set<Chunk*> m_occupiedChunks, m_freeChunks;
	WorldGenerator m_worldGenerator;
};
