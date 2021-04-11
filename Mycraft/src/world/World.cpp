#include "World.h"

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <array>

#include "../Camera.h"
#include "chunk_worker/ChunkWorker.h"
extern Camera g_camera;
extern int g_renderDistance;

//extern std::queue<Chunk*> g_chunkLoad_job_queue;
extern std::queue<ChunkWrapper*> g_chunks_requiring_buffer_update_queue;
extern std::mutex g_job_mutex, g_chunks_requiring_buffer_update_queue_mutex;
//extern std::condition_variable g_cv;

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

	std::unique_lock lock(m_chunksAccessMutex);
	
	std::vector<ChunkWrapper> list(1.5 * chunkCount);
	m_chunks.swap(list);

	for (auto& chunk : m_chunks) {
		m_freeChunks.insert(&chunk);
		chunk.SetWorld(this);
	}

	lock.unlock();
}

ChunkWrapper* World::LoadChunk(ChunkPos pos)
{
	std::lock_guard lock(m_chunksAccessMutex);
	
	auto chunkIt = m_freeChunks.begin();
	ChunkWrapper* chunkWrapper = *chunkIt;
	
	m_freeChunks.erase(chunkIt);
	m_occupiedChunks.insert(chunkWrapper);
	
	chunkWrapper->SetPos(pos);
	auto chunk = chunkWrapper->GetWritePtr(true);
	chunk->loadingState = Chunk::LoadingState::loading_blocks;
	m_chunkMap[pos] = chunkWrapper;

	return chunkWrapper;
}

void World::PopulateChunk(Chunk& chunk)
{
	m_worldGenerator.GenerateChunk(chunk);
	chunk.loadingState = Chunk::LoadingState::loaded_blocks;
}

void World::UnloadChunk(ChunkWrapper& chunkWrapper)
{
	auto chunk = chunkWrapper.GetWritePtr(true);

	std::lock_guard lock(m_chunksAccessMutex);
	auto chunkIt = m_chunkMap.find({ chunk->GetPos().x, chunk->GetPos().z });
	m_chunkMap.erase(chunkIt);

	m_occupiedChunks.erase(&chunkWrapper);
	m_freeChunks.insert(&chunkWrapper);

	chunk->Clear();
}

ChunkWrapper* World::GetChunk(int x, int z)
{
	//std::lock_guard lock(m_chunksAccessMutex);
	return m_chunkMap[{x, z}];
}

ChunkWrapper* World::GetChunkAt(BlockPos pos)
{
	//std::lock_guard lock(m_chunksAccessMutex);
	return m_chunkMap[{pos.x >> 5, pos.z >> 5}];
}


//This might be dangerous to use
Block* World::GetBlock(BlockPos pos)
{
	ChunkWrapper* chunkWrapper = GetChunkAt(pos);
	if (!chunkWrapper)
		return Blocks::air;
	return chunkWrapper->GetReadPtr(true)->GetBlock(pos);
}

void World::SetBlock(BlockPos pos, uint16_t blockId)
{
	ChunkWrapper* chunkWrapper = GetChunkAt(pos);
	if (!chunkWrapper)
		return;
	chunkWrapper->GetReadPtr(true)->SetBlock(pos, blockId);
}

void World::Update()
{
	//make this once on heap
	std::vector<ChunkWrapper*> chunksToUnload;
	std::unique_lock<std::mutex> g0(g_chunks_requiring_buffer_update_queue_mutex);
	for (ChunkWrapper* chunkWrapper : m_occupiedChunks) {
		if (!chunkWrapper->CanBeUnloaded())
			continue;

		float distance = std::max(abs(g_camera.position.x + 16 - chunkWrapper->GetPos().x * Chunk::CHUNK_WIDTH), abs(g_camera.position.z + 16 - chunkWrapper->GetPos().z * Chunk::CHUNK_WIDTH));
		if (distance > (g_renderDistance + 2) * Chunk::CHUNK_WIDTH)
			chunksToUnload.push_back(chunkWrapper);
	}
	g0.unlock();
	
	for (auto chunkWrapper : chunksToUnload) {
		//scope so that chunk is released before it's unloaded
		{
			auto chunk = chunkWrapper->GetReadPtr(true);
			g_polygons -= chunk->m_polygonCount;
		}
		UnloadChunk(*chunkWrapper);
	}

	std::unique_lock<std::mutex> g1(g_chunks_requiring_buffer_update_queue_mutex);
	auto queue = g_chunks_requiring_buffer_update_queue;
	g1.unlock();

	
	//danger zone
	int popped_items = 0;
	
	while (!queue.empty()) {
		auto chunkWrapper = queue.front();
		queue.pop();
		popped_items++;

		assert(chunkWrapper->loadingState.load() == Chunk::LoadingState::generated_mesh);

		auto chunk = chunkWrapper->GetWritePtr(true);
		chunk->InitializeBuffers();
		chunk->loadingState = Chunk::LoadingState::completed;
		g_polygons += chunk->m_polygonCount;
	}

	g1.lock();
	for(int i = 0; i < popped_items; i++)
	{
		g_chunks_requiring_buffer_update_queue.pop();
	}
	g1.unlock();

	int cameraChunkX = (int)g_camera.position.x >> 5;
	int cameraChunkZ = (int)g_camera.position.z >> 5;

	//powinno byæ na swoim w¹tku???
	//std::lock_guard g2(g_job_mutex);
	std::lock_guard lock(m_chunksAccessMutex);
	for (auto chunkPos : m_nearbyChunksPositions) {
		chunkPos.x += cameraChunkX;
		chunkPos.z += cameraChunkZ;
		if (m_chunkMap[chunkPos] == nullptr) {
			LoadChunk(chunkPos);
			//g_chunkLoad_job_queue.push(chunk);
		}
	}

	/*if (!g_chunkLoad_job_queue.empty())
		g_cv.notify_all();*/
}

void World::Render()
{
	std::unique_lock lock(m_chunksAccessMutex);
	auto set = m_occupiedChunks;
	lock.unlock();

	for (ChunkWrapper* chunkWrapper : set)
	{
		//std::cout << "Unlocking chunk " << chunkWrapper->GetPos().x << " x " << chunkWrapper->GetPos().z << "... ";
		ChunkPos pos = chunkWrapper->GetPos();
		chunkWrapper->GetReadPtr(true);
		//std::cout << "Done!\n";
	}
	
	for (ChunkWrapper *chunkWrapper : set)
		chunkWrapper->GetReadPtr(true)->Render();
	//std::cout << "Frame rendered!\n";
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

	for (ChunkWrapper* chunkWrapper : occupiedChunks) {
		if (chunkWrapper->CanBeUnloaded()) {
			g_polygons -= chunkWrapper->GetReadPtr(true)->m_polygonCount;
			UnloadChunk(*chunkWrapper);
		}
	}
}

std::array<int, 5> World::DEV_ChunksLoadingStates()
{
	std::array<int, 5> stateCount{ 0 };
	std::lock_guard lock(m_chunksAccessMutex);
	for (ChunkWrapper* chunkWrapper : m_occupiedChunks)
	{
		auto state = chunkWrapper->GetReadPtr(true)->loadingState.load();
		stateCount[static_cast<int>(state)]++;
	}

	return stateCount;
}

std::unordered_map<ChunkPos, ChunkWrapper*, ChunkPos::HashFunction> World::GetChunksMap()
{
	std::lock_guard lock(m_chunksAccessMutex);
	return m_chunkMap;
}

/*std::unordered_set<ChunkWrapper*> World::GetOccupiedChunks()
{
	std::lock_guard();
	return m_occupiedChunks;
}*/

