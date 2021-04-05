#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "chunk/Chunk.h"
#include "block/BlockPos.h"
#include "worldgen/WorldGenerator.h"

class World
{
public:
	World();

	Chunk* LoadChunk(ChunkPos pos);
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

	void DEV_UnloadWorld();
private:
	std::vector<Chunk> m_chunks;
	std::unordered_map<ChunkPos, Chunk*, ChunkPos::HashFunction> m_chunkMap;
	std::unordered_set<Chunk*> m_occupiedChunks, m_freeChunks;

	WorldGenerator m_worldGenerator;

	std::vector<ChunkPos> m_nearbyChunksPositions;
};
