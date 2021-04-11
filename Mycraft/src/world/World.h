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

	ChunkWrapper* LoadChunk(ChunkPos pos);
	void PopulateChunk(Chunk& chunk);
	void UnloadChunk(ChunkWrapper& chunkWrapper);
	ChunkWrapper* GetChunk(int x, int z);
	ChunkWrapper* GetChunkAt(BlockPos pos);

	Block* GetBlock(BlockPos pos);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void Update();
	void Render();

	int FreeChunkCount();
	int OccupiedChunkCount();

	void DEV_UnloadWorld();
	std::array<int, 5> DEV_ChunksLoadingStates();

	//std::unordered_set<ChunkWrapper*> GetOccupiedChunks();
	std::unordered_map<ChunkPos, ChunkWrapper*, ChunkPos::HashFunction> GetChunksMap();
private:
	std::recursive_mutex m_chunksAccessMutex;
	std::vector<ChunkWrapper> m_chunks;
	std::unordered_map<ChunkPos, ChunkWrapper*, ChunkPos::HashFunction> m_chunkMap;
	std::unordered_set<ChunkWrapper*> m_occupiedChunks, m_freeChunks;
	
	WorldGenerator m_worldGenerator;
	std::vector<ChunkPos> m_nearbyChunksPositions;
};
