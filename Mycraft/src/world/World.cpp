#include "World.h"

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <array>

#include "../Camera.h"
extern Camera g_camera;
extern int g_renderDistance;

extern std::queue<Chunk*> g_chunkLoad_job_queue;
extern std::queue<Chunk*> g_chunkLoad_completed_job_queue;
extern std::mutex g_job_mutex, g_completed_job_mutex;
extern std::condition_variable g_cv;

extern unsigned int g_polygons;

std::pair<int, int> spiral(int n)
{
	int k = ceil((sqrt(n) - 1) / 2);
	int t = 2 * k + 1;
	int m = t * t;
	t--;
	if (n >= m - t)
		return { k - (m - n), -k };
	else
		m = m - t;

	if (n >= m - t)
		return { -k,-k + (m - n) };
	else
		m = m - t;

	if (n >= m - t)
		return { -k + (m - n),k };
	else
		return { k,k - (m - n - t) };
}

World::World() :
	m_worldGenerator(this)
{
	int chunkCount = g_renderDistance * g_renderDistance * 4;

	for (int i = 0; i < chunkCount; i++) {
		m_nearbyChunksPositions.push_back({ spiral(i + 1).first,spiral(i + 1).second });
	}

	m_chunks.resize(1.5 * chunkCount);

	for (Chunk& chunk : m_chunks) {
		m_freeChunks.insert(&chunk);
		chunk.SetWorld(this);
	}
}

Chunk* World::LoadChunk(ChunkPos pos)
{
	auto chunkIt = m_freeChunks.begin();
	Chunk* chunk = *chunkIt;

	m_freeChunks.erase(chunkIt);
	m_occupiedChunks.insert(chunk);
	m_chunkMap[pos] = chunk;

	chunk->SetPos(pos);
	return chunk;
}

void World::PopulateChunk(Chunk& chunk)
{
	m_worldGenerator.GenerateChunk(chunk);
	chunk.loadingState = Chunk::LoadingState::loaded_blocks;
}

void World::UnloadChunk(Chunk& chunk)
{
	auto chunkIt = m_chunkMap.find({ chunk.GetPos().x, chunk.GetPos().z });
	m_chunkMap.erase(chunkIt);

	m_occupiedChunks.erase(&chunk);
	m_freeChunks.insert(&chunk);

	chunk.Clear();
}

Chunk* World::GetChunk(int x, int z)
{
	return m_chunkMap[{x, z}];
}

Chunk* World::GetChunkAt(BlockPos pos)
{
	return m_chunkMap[{pos.x >> 5, pos.z >> 5}];
}

Block* World::GetBlock(BlockPos pos)
{
	Chunk* chunk = GetChunkAt(pos);
	if (chunk)
		return chunk->GetBlock(pos);
	return Blocks::air;
}

void World::SetBlock(BlockPos pos, uint16_t blockId)
{
	Chunk* chunk = GetChunkAt(pos);
	if (chunk)
		chunk->SetBlock(pos, blockId);
}

void World::Update()
{
	std::vector<Chunk*> chunksToUnload;
	for (Chunk* chunk : m_occupiedChunks) {
		if (!chunk->CanBeUnloaded())
			continue;

		float distance = std::max(abs(g_camera.position.x + 16 - chunk->GetPos().x * Chunk::CHUNK_WIDTH), abs(g_camera.position.z + 16 - chunk->GetPos().z * Chunk::CHUNK_WIDTH));
		if (distance > (g_renderDistance + 2) * Chunk::CHUNK_WIDTH)
			chunksToUnload.push_back(chunk);
	}
	for (Chunk* chunk : chunksToUnload) {
		g_polygons -= chunk->m_polygonCount;
		UnloadChunk(*chunk);
	}

	std::unique_lock<std::mutex> g1(g_completed_job_mutex);
	while (g_chunkLoad_completed_job_queue.size() > 0) {
		Chunk* chunk = g_chunkLoad_completed_job_queue.front();
		g_chunkLoad_completed_job_queue.pop();

		switch (chunk->loadingState) {
		case Chunk::LoadingState::loaded_blocks:
		{
			Chunk* adjacentChunks[4];
			FOREACH_CARDINAL_DIRECTION(auto constexpr direction, {
				adjacentChunks[direction] = m_chunkMap[chunk->GetPos().Adjacent<direction>()];
				});

			FOREACH_CARDINAL_DIRECTION(auto constexpr direction, {
				if (adjacentChunks[direction] && adjacentChunks[direction]->loadingState >= Chunk::LoadingState::loaded_blocks) {
					adjacentChunks[direction]->adjacentChunks[Direction::Opposite<direction>()] = chunk;
					chunk->adjacentChunks[direction] = adjacentChunks[direction];

					if (adjacentChunks[direction]->CanGenerateMesh()) {
						adjacentChunks[direction]->loadingState = Chunk::LoadingState::generating_mesh;
						g_chunkLoad_job_queue.push(adjacentChunks[direction]);
					}
				}
				});

			if (chunk->CanGenerateMesh()) {
				chunk->loadingState = Chunk::LoadingState::generating_mesh;
				g_chunkLoad_job_queue.push(chunk);
			}
		}
		break;

		case Chunk::LoadingState::generating_mesh:
			chunk->InitializeBuffers();
			chunk->loadingState = Chunk::LoadingState::completed;
			g_polygons += chunk->m_polygonCount;
			break;
		}
	}
	g1.unlock();

	int cameraChunkX = (int)g_camera.position.x >> 5;
	int cameraChunkZ = (int)g_camera.position.z >> 5;

	std::lock_guard<std::mutex> g2(g_job_mutex);
	for (auto chunkPos : m_nearbyChunksPositions) {
		chunkPos.x += cameraChunkX;
		chunkPos.z += cameraChunkZ;
		if (m_chunkMap[chunkPos] == nullptr) {
			Chunk* chunk = LoadChunk(chunkPos);
			g_chunkLoad_job_queue.push(chunk);
		}
	}

	if (!g_chunkLoad_job_queue.empty())
		g_cv.notify_all();
}

void World::Render()
{
	for (const Chunk& chunk : m_chunks)
		chunk.Render();
}

int World::FreeChunkCount()
{
	return m_freeChunks.size();
}

int World::OccupiedChunkCount()
{
	return m_occupiedChunks.size();
}

void World::DEV_UnloadWorld()
{
	std::vector occupiedChunks(m_occupiedChunks.begin(), m_occupiedChunks.end());

	for (Chunk* chunk : occupiedChunks) {
		if (chunk->CanBeUnloaded()) {
			g_polygons -= chunk->m_polygonCount;
			UnloadChunk(*chunk);
		}
	}
}

std::array<int, 4> World::DEV_ChunksLoadingStates()
{
	std::array<int, 4> stateCount{ 0 };

	for (const Chunk* chunk : m_occupiedChunks)
		stateCount[static_cast<int>(chunk->loadingState)]++;

	return stateCount;
}

