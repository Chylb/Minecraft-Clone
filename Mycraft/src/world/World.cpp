#include "World.h"

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

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
		m_nearbyChunksPositions.push_back(spiral(i + 1));
	}

	m_chunks.resize(1.5 * chunkCount);

	for (Chunk& chunk : m_chunks) {
		m_freeChunks.insert(&chunk);
		chunk.SetWorld(this);
	}
}

Chunk* World::LoadChunk(int x, int z)
{
	auto chunkIt = m_freeChunks.begin();
	Chunk* chunk = *chunkIt;

	m_freeChunks.erase(chunkIt);
	m_occupiedChunks.insert(chunk);
	m_chunkMap[{x, z}] = chunk;

	chunk->SetPos(x, z);
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
			auto northChunk = m_chunkMap[{chunk->GetPos().x, chunk->GetPos().z - 1}];
			auto eastChunk = m_chunkMap[{chunk->GetPos().x + 1, chunk->GetPos().z}];
			auto southChunk = m_chunkMap[{chunk->GetPos().x, chunk->GetPos().z + 1}];
			auto westChunk = m_chunkMap[{chunk->GetPos().x - 1, chunk->GetPos().z}];

			if (northChunk && northChunk->loadingState >= Chunk::LoadingState::loaded_blocks) {
				northChunk->southChunk = chunk;
				chunk->northChunk = northChunk;
				if (northChunk->CanGenerateMesh()) {
					northChunk->loadingState = Chunk::LoadingState::generating_mesh;
					g_chunkLoad_job_queue.push(northChunk);
				}
			}
			if (eastChunk && eastChunk->loadingState >= Chunk::LoadingState::loaded_blocks) {
				eastChunk->westChunk = chunk;
				chunk->eastChunk = eastChunk;
				if (eastChunk->CanGenerateMesh()) {
					eastChunk->loadingState = Chunk::LoadingState::generating_mesh;
					g_chunkLoad_job_queue.push(eastChunk);
				}
			}
			if (southChunk && southChunk->loadingState >= Chunk::LoadingState::loaded_blocks) {
				southChunk->northChunk = chunk;
				chunk->southChunk = southChunk;
				if (southChunk->CanGenerateMesh()) {
					southChunk->loadingState = Chunk::LoadingState::generating_mesh;
					g_chunkLoad_job_queue.push(southChunk);
				}
			}
			if (westChunk && westChunk->loadingState >= Chunk::LoadingState::loaded_blocks) {
				westChunk->eastChunk = chunk;
				chunk->westChunk = westChunk;
				if (westChunk->CanGenerateMesh()) {
					westChunk->loadingState = Chunk::LoadingState::generating_mesh;
					g_chunkLoad_job_queue.push(westChunk);
				}
			}

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
		chunkPos.first += cameraChunkX;
		chunkPos.second += cameraChunkZ;
		if (m_chunkMap[chunkPos] == nullptr) {
			Chunk* chunk = LoadChunk(chunkPos.first, chunkPos.second);
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
