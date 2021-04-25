#include "World.h"

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <array>
#include <set>

#include "../utils/MathUtils.h"

#include "../Camera.h"
extern Camera g_camera;
extern int g_renderDistance;

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
	if (chunk) {
		chunk->SetBlock(pos, blockId);

		chunk->dirtyMesh = true;
		for (int i = 0; i < 4; i++)
			chunk->adjacentChunks[i]->dirtyMesh = true;
	}
}

void World::Update()
{
	std::vector<Chunk*> chunksToUnload;
	for (Chunk* chunk : m_occupiedChunks) {
		if (chunk->loadingState == Chunk::LoadingState::completed) {
			chunk->Tick();
		}

		if (!chunk->CanBeUnloaded())
			continue;

		float distance = std::max(abs(g_camera.position.x + 16 - chunk->GetPos().x * Chunk::CHUNK_WIDTH), abs(g_camera.position.z + 16 - chunk->GetPos().z * Chunk::CHUNK_WIDTH));
		if (distance > (g_renderDistance + 2) * Chunk::CHUNK_WIDTH)
			chunksToUnload.push_back(chunk);
	}
	for (Chunk* chunk : chunksToUnload) {
		g_polygons -= chunk->m_polygonCount;
		chunk->loadingState = Chunk::LoadingState::loading_blocks;
		UnloadChunk(*chunk);
	}

	int cameraChunkX = (int)g_camera.position.x >> 5;
	int cameraChunkZ = (int)g_camera.position.z >> 5;

	std::queue<Chunk*> chunksToLoad;
	for (auto chunkPos : m_nearbyChunksPositions) {
		chunkPos.x += cameraChunkX;
		chunkPos.z += cameraChunkZ;
		if (m_chunkMap[chunkPos] == nullptr) {
			Chunk* chunk = LoadChunk(chunkPos);

			chunksToLoad.push(chunk);
		}
	}

	while (chunksToLoad.size()) {
		Chunk* chunk = chunksToLoad.front();
		chunksToLoad.pop();

		this->PopulateChunk(*chunk);
		chunk->loadingState = Chunk::LoadingState::loaded_blocks;
		chunk->dirtyMesh = true;

		Chunk* adjacentChunks[4];
		FOREACH_CARDINAL_DIRECTION(auto constexpr direction, {
			adjacentChunks[direction] = m_chunkMap[chunk->GetPos().Adjacent<direction>()];
			});

		FOREACH_CARDINAL_DIRECTION(auto constexpr direction, {
			if (adjacentChunks[direction] && adjacentChunks[direction]->loadingState >= Chunk::LoadingState::loaded_blocks) {
				adjacentChunks[direction]->adjacentChunks[Direction::Opposite<direction>()] = chunk;
				chunk->adjacentChunks[direction] = adjacentChunks[direction];
			}
			});
	}
}

void World::UpdateMeshes()
{
	for (Chunk* chunk : m_occupiedChunks) {
		if (chunk->dirtyMesh && chunk->CanGenerateMesh()) {
			g_polygons -= chunk->m_polygonCount;
			chunk->ClearMesh();
			chunk->GenerateMesh();
			chunk->InitializeBuffers();
			chunk->dirtyMesh = false;
			chunk->loadingState = Chunk::LoadingState::completed;
			g_polygons += chunk->m_polygonCount;
		}
	}
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

std::tuple<bool, BlockPos, Direction::Direction> World::DoBlockRayTrace(glm::vec3 start, glm::vec3 end)
{
	int i = floor(start.x);
	int j = floor(start.y);
	int k = floor(start.z);

	auto diff = end - start;

	float dirX = MathUtils::signum(diff.x);
	float dirY = MathUtils::signum(diff.y);
	float dirZ = MathUtils::signum(diff.z);

	float stepX = dirX == 0 ? std::numeric_limits<float>::max() : dirX / diff.x;
	float stepY = dirY == 0 ? std::numeric_limits<float>::max() : dirY / diff.y;
	float stepZ = dirZ == 0 ? std::numeric_limits<float>::max() : dirZ / diff.z;

	float traveledX = stepX * (dirX > 0 ? 1.0 - MathUtils::frac(start.x) : MathUtils::frac(start.x));
	float traveledY = stepY * (dirY > 0 ? 1.0 - MathUtils::frac(start.y) : MathUtils::frac(start.y));
	float traveledZ = stepZ * (dirZ > 0 ? 1.0 - MathUtils::frac(start.z) : MathUtils::frac(start.z));

	enum class Axis
	{
		x, y, z
	};
	Axis lastAxis;
	Direction::Direction hitFace;
	bool hit = false;

	while (traveledX <= 1.0 || traveledY <= 1.0 || traveledZ <= 1.0) {
		if (traveledX < traveledY) {
			if (traveledX < traveledZ) {
				i += dirX;
				traveledX += stepX;
				lastAxis = Axis::x;
			}
			else {
				k += dirZ;
				traveledZ += stepZ;
				lastAxis = Axis::z;
			}
		}
		else if (traveledY < traveledZ) {
			j += dirY;
			traveledY += stepY;
			lastAxis = Axis::y;
		}
		else {
			k += dirZ;
			traveledZ += stepZ;
			lastAxis = Axis::z;
		}

		if (GetBlock({ i,j,k })->IsOpaque()) {
			hit = true;
			switch (lastAxis) {
			case Axis::x:
				dirX > 0 ? hitFace = Direction::west : hitFace = Direction::east;
				break;
			case Axis::y:
				dirY > 0 ? hitFace = Direction::bottom : hitFace = Direction::top;
				break;
			default:
				dirZ > 0 ? hitFace = Direction::north : hitFace = Direction::south;
				break;
			}

			break;
		}
	}
	return { hit, {i,j,k}, hitFace };
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

