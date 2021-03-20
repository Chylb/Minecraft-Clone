#include "World.h"

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
	int renderDistance = 4;
	int chunkCount = renderDistance * renderDistance * 4;
	//int chunkCount = 1;

	m_chunks.resize(chunkCount);

	for (Chunk& chunk : m_chunks)
		chunk.SetWorld(this);

	for (int i = 0; i < chunkCount; i++)
		m_freeChunks.insert(&m_chunks[i]);

	for (int i = 0; i < chunkCount; i++) {
		auto pos = spiral(i + 1);
		LoadChunk(pos.first, pos.second);
	}
}

void World::LoadChunk(int x, int z)
{
	auto chunkIt = m_freeChunks.begin();
	Chunk* chunk = *chunkIt;

	m_freeChunks.erase(chunkIt);
	m_occupiedChunks.insert(chunk);
	m_chunkMap[{x, z}] = chunk;

	chunk->SetPos(x, z);

	m_worldGenerator.GenerateChunk(*chunk);
	chunk->GenerateMesh();
	chunk->InitializeBuffers();
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

void World::Render()
{
	for (const Chunk& chunk : m_chunks)
		chunk.Render();
}
