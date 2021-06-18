#pragma once

#include <functional>
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "chunk/Chunk.h"
#include "block/BlockPos.h"
#include "worldgen/WorldGenerator.h"
#include "../utils/math/BlockRayTraceResult.h"

class World
{
public:
	World();

	Chunk* LoadChunk(ChunkPos pos);
	void PopulateChunk(Chunk& chunk);
	void UnloadChunk(Chunk& chunk);
	Chunk* GetChunk(int x, int z);
	Chunk* GetChunkAt(BlockPos pos) const;

	const BlockState* GetBlockState(BlockPos pos) const;
	void SetBlock(BlockPos pos, const BlockState* state);

	void Update();
	void UpdateMeshes();
	void Render();

	int FreeChunkCount();
	int OccupiedChunkCount();

	BlockRayTraceResult Clip(glm::vec3 from, glm::vec3 to);
	BlockRayTraceResult TraverseBlocks(glm::vec3 from, glm::vec3 to, std::function<BlockRayTraceResult(glm::vec3 from, glm::vec3 rayDir, BlockPos pos)> hitFunction);

	void DEV_UnloadWorld();
	std::array<int, 4> DEV_ChunksLoadingStates();

private:
	std::vector<Chunk> m_chunks;
	std::unordered_map<ChunkPos, Chunk*, ChunkPos::HashFunction> m_chunkMap;
	std::unordered_set<Chunk*> m_occupiedChunks, m_freeChunks;

	WorldGenerator m_worldGenerator;

	std::vector<ChunkPos> m_nearbyChunksPositions;
};
