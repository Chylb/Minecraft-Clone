#include "World.h"

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <array>
#include <functional>
#include <set>

#include "../utils/MathUtils.h"

#include "../Camera.h"
#include "../utils/math/shapes/VoxelShape.h"
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
	m_worldGenerator(this),
	m_blockTicks(this)
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

Chunk* World::GetChunkAt(BlockPos pos) const
{
	return m_chunkMap.at({ pos.x >> 5, pos.z >> 5 });
}

const BlockState* World::GetBlockState(BlockPos pos) const
{
	Chunk* chunk = GetChunkAt(pos);
	if (chunk)
		return chunk->GetBlockState(pos);
	return Blocks::air->DefaultBlockState();
}

/*
Sets a block state into this world.Flags are as follows :
1 will cause a block update.
2 will send the change to clients.
4 will prevent the block from being re - rendered.
8 will force any re - renders to run on the main thread instead
16 will prevent neighbor reactions(e.g.fences connecting, observers pulsing).
32 will prevent neighbor reactions from spawning drops.
64 will signify the block is being moved.Flags can be OR - ed
*/
void World::SetBlock(BlockPos pos, const BlockState& state, int flags)
{
	Chunk* chunk = GetChunkAt(pos);
	if (chunk) {
		auto& oldState = *GetBlockState(pos);
		if (&oldState == &state)
			return;

		chunk->SetBlock(pos, &state);
		oldState.OnRemove(*this, pos, state);
		state.OnPlace(*this, pos, oldState);

		const auto& newerState = *GetBlockState(pos);
		if (&newerState == &state) {
			if (flags & 1)
				UpdateNeighborsAt(pos, oldState.GetBlock());

			if ((flags & 16) == 0) {
				int newFlags = flags & -34; //remove 32 and 1
				oldState.UpdateIndirectNeighbourShapes(*this, pos, newFlags);
				state.UpdateNeighbourShapes(*this, pos, newFlags);
				state.UpdateIndirectNeighbourShapes(*this, pos, newFlags);
			}
		}
		chunk->dirtyMesh = true;
		for (int i = 0; i < 4; i++)
			chunk->adjacentChunks[i]->dirtyMesh = true;
	}
}

void World::UpdateNeighborsAt(BlockPos pos, const Block& block)
{
	NeighborChanged(pos.West(), block, pos);
	NeighborChanged(pos.East(), block, pos);
	NeighborChanged(pos.Below(), block, pos);
	NeighborChanged(pos.Above(), block, pos);
	NeighborChanged(pos.North(), block, pos);
	NeighborChanged(pos.South(), block, pos);
}

void World::NeighborChanged(BlockPos updatedPos, const Block& updaterBlock, BlockPos updaterPos)
{
	const BlockState& state = *GetBlockState(updatedPos);
	state.NeighborChanged(*this, updatedPos, updaterBlock, updaterPos);
}

int World::GetBestNeighborSignal(BlockPos pos) const
{
	int maxSignal = 0;

	for (Direction dir : Direction::directions)
	{
		int signal = GetSignal(pos.Adjacent(dir), dir);
		if (signal >= 15)
			return 15;

		if (maxSignal > signal)
			maxSignal = signal;
	}
	return maxSignal;
}

int World::GetSignal(BlockPos pos, Direction dir) const
{
	auto& state = *GetBlockState(pos);
	int signal = state.GetSignal(*this, pos, dir);
	return state.IsRedstoneConductor(*this, pos) ? std::max(signal, GetDirectSignalTo(pos)) : signal;
}

int World::GetDirectSignal(BlockPos pos, Direction dir) const
{
	return GetBlockState(pos)->GetDirectSignal(*this, pos, dir);
}

int World::GetDirectSignalTo(BlockPos pos) const
{
	int maxSignal = 0;
	for (Direction dir : Direction::directions)
	{
		int signal = GetDirectSignal(pos.Adjacent(dir), dir);
		if (signal >= 15)
			return 15;

		if (signal > maxSignal)
			maxSignal = signal;
	}
	return maxSignal;
}

bool World::HasNeighborSignal(BlockPos pos) const
{
	return GetBestNeighborSignal(pos);
}

TickList<Block>& World::GetBlockTicks()
{
	return m_blockTicks;
}

uint64_t World::GetTick() const
{
	return m_tick;
}

void World::Update()
{
	std::vector<Chunk*> chunksToUnload;
	for (Chunk* chunk : m_occupiedChunks) {
		/*if (chunk->loadingState == Chunk::LoadingState::completed) {
			chunk->Tick();
		}*/

		if (!chunk->CanBeUnloaded())
			continue;

		float distance = std::max(abs(g_camera.position.x + 16 - chunk->GetPos().x * Chunk::CHUNK_WIDTH), abs(g_camera.position.z + 16 - chunk->GetPos().z * Chunk::CHUNK_WIDTH));
		if (distance > (g_renderDistance + 2) * Chunk::CHUNK_WIDTH)
			chunksToUnload.push_back(chunk);
	}
	for (Chunk* chunk : chunksToUnload) {
		g_polygons -= chunk->VertexCount() / 3;
		chunk->loadingState = Chunk::LoadingState::loading_blocks;
		UnloadChunk(*chunk);
	}

	m_blockTicks.Tick();

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
		FOREACH_HORIZONTAL_DIRECTION(constexpr Direction direction, {
			adjacentChunks[direction] = m_chunkMap[chunk->GetPos().Adjacent(direction)];
			});

		FOREACH_HORIZONTAL_DIRECTION(constexpr Direction direction, {
			if (adjacentChunks[direction] && adjacentChunks[direction]->loadingState >= Chunk::LoadingState::loaded_blocks) {
				adjacentChunks[direction]->adjacentChunks[direction.GetOpposite()] = chunk;
				chunk->adjacentChunks[direction] = adjacentChunks[direction];
			}
			});
	}
	m_tick++;
}

void World::UpdateMeshes()
{
	for (Chunk* chunk : m_occupiedChunks) {
		if (chunk->dirtyMesh && chunk->CanGenerateMesh()) {
			g_polygons -= chunk->VertexCount() / 3;
			chunk->ClearMesh();
			chunk->GenerateMesh();
			chunk->InitializeBuffers();
			chunk->dirtyMesh = false;
			chunk->loadingState = Chunk::LoadingState::completed;
			g_polygons += chunk->VertexCount() / 3;
		}
	}
}

int World::FreeChunkCount()
{
	return m_freeChunks.size();
}

int World::OccupiedChunkCount()
{
	return m_occupiedChunks.size();
}

const std::vector<Chunk>& World::GetChunks() const
{
	return m_chunks;
}

BlockRayTraceResult World::Clip(glm::vec3 from, glm::vec3 to)
{
	return TraverseBlocks(from, to, [&](glm::vec3 from, glm::vec3 rayDir, BlockPos pos)
		{
			auto state = GetBlockState(pos);
			auto& shape = state->GetBlock().GetShape(*state);
			return shape.Clip(from, rayDir, pos);
		});
}

BlockRayTraceResult World::TraverseBlocks(glm::vec3 from, glm::vec3 to, std::function<BlockRayTraceResult(glm::vec3 from, glm::vec3 rayDir, BlockPos pos)> hitFunction)
{
	int i = static_cast<int>(floor(from.x));
	int j = static_cast<int>(floor(from.y));
	int k = static_cast<int>(floor(from.z));

	auto rayDir = to - from;

	int dirX = MathUtils::signum(rayDir.x);
	int dirY = MathUtils::signum(rayDir.y);
	int dirZ = MathUtils::signum(rayDir.z);

	float stepX = dirX == 0 ? std::numeric_limits<float>::max() : dirX / rayDir.x;
	float stepY = dirY == 0 ? std::numeric_limits<float>::max() : dirY / rayDir.y;
	float stepZ = dirZ == 0 ? std::numeric_limits<float>::max() : dirZ / rayDir.z;

	float traveledX = stepX * (dirX > 0 ? 1.0f - MathUtils::frac(from.x) : MathUtils::frac(from.x));
	float traveledY = stepY * (dirY > 0 ? 1.0f - MathUtils::frac(from.y) : MathUtils::frac(from.y));
	float traveledZ = stepZ * (dirZ > 0 ? 1.0f - MathUtils::frac(from.z) : MathUtils::frac(from.z));

	while (traveledX <= 1.0 || traveledY <= 1.0 || traveledZ <= 1.0) {
		if (traveledX < traveledY) {
			if (traveledX < traveledZ) {
				i += dirX;
				traveledX += stepX;
			}
			else {
				k += dirZ;
				traveledZ += stepZ;
			}
		}
		else if (traveledY < traveledZ) {
			j += dirY;
			traveledY += stepY;
		}
		else {
			k += dirZ;
			traveledZ += stepZ;
		}

		BlockRayTraceResult result = hitFunction(from, rayDir, { i,j,k });
		if (result.hit)
			return result;
	}
	return { .hit = false };
}

void World::DEV_UnloadWorld()
{
	std::vector occupiedChunks(m_occupiedChunks.begin(), m_occupiedChunks.end());

	for (Chunk* chunk : occupiedChunks) {
		if (chunk->CanBeUnloaded()) {
			g_polygons -= chunk->VertexCount() / 3;
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

void World::TickBlock(const NextTickListEntry<Block>& entry)
{
	auto& state = *GetBlockState(entry.pos);
	if (state.Is(entry.type))
	{
		state.Tick(*this, entry.pos);
	}
}

