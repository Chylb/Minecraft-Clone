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

	Chunk* LoadChunk(int x, int z);
	void PopulateChunk(Chunk& chunk);
	void UnloadChunk(Chunk& chunk);
	Chunk* GetChunk(int x, int z);
	Chunk* GetChunkAt(BlockPos pos);

	Block* GetBlock(BlockPos pos);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void Update();
	void Render();

	int FreeChunkCount();
	int OccupiedChunkCount();
private:
	std::vector<Chunk> m_chunks;
	std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> m_chunkMap;
	std::unordered_set<Chunk*> m_occupiedChunks, m_freeChunks;

	WorldGenerator m_worldGenerator;

	std::vector<std::pair<int, int>> m_nearbyChunksPositions;
};
