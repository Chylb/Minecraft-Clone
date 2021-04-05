#include "Chunk.h"

#include "../World.h"

Chunk::Chunk()
{
	Clear();
	m_mesh.reserve(32 * 32 * 6 * sizeof(float) * 2);
}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

Block* Chunk::LocalGetBlock(BlockPos pos) const
{
	if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
		return Blocks::air;
	return Blocks::GetBlock(m_data[pos.x][pos.z][pos.y]);
}

template<Direction::Direction dir>
Block* Chunk::GetNearbyBlock(BlockPos pos) const
{
	if constexpr (dir == Direction::north) {
		if (pos.z < m_offsetZ)
			return adjacentChunks[dir]->GetBlock(pos);
	}
	if constexpr (dir == Direction::east) {
		if (pos.x >= m_offsetX + CHUNK_WIDTH)
			return adjacentChunks[dir]->GetBlock(pos);
	}
	if constexpr (dir == Direction::south) {
		if (pos.z >= m_offsetZ + CHUNK_WIDTH)
			return adjacentChunks[dir]->GetBlock(pos);
	}
	if constexpr (dir == Direction::west) {
		if (pos.x < m_offsetX)
			return adjacentChunks[dir]->GetBlock(pos);
	}

	return GetBlock(pos);
}


Block* Chunk::GetBlock(BlockPos pos) const
{
	int x = pos.x & 31, z = pos.z & 31;
	if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
		return Blocks::air;
	return Blocks::GetBlock(m_data[x][z][pos.y]);
}

void Chunk::LocalSetBlock(BlockPos pos, uint16_t blockId)
{
	m_data[pos.x][pos.z][pos.y] = blockId;
}

void Chunk::SetBlock(BlockPos pos, uint16_t blockId)
{
	int x = pos.x & 31, z = pos.z & 31;
	m_data[x][z][pos.y] = blockId;
}

void Chunk::SetWorld(World* world)
{
	m_world = world;
}

ChunkPos Chunk::GetPos() const
{
	return m_pos;
}

void Chunk::SetPos(ChunkPos pos)
{
	m_pos = pos;
	m_offsetX = pos.x * CHUNK_WIDTH;
	m_offsetZ = pos.z * CHUNK_WIDTH;
}

bool Chunk::CanGenerateMesh()
{
	return loadingState == LoadingState::loaded_blocks && adjacentChunks[Direction::north] && adjacentChunks[Direction::east] && adjacentChunks[Direction::south] && adjacentChunks[Direction::west];
}

void Chunk::GenerateMesh()
{
	m_mesh.clear();

	for (int x = m_offsetX + 1; x < m_offsetX + CHUNK_WIDTH - 1; x++)
		for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++) {
			GenerateColumnMesh(x, z);
		}
	for (int z : {m_offsetZ, m_offsetZ + CHUNK_WIDTH - 1})
		for (int x = m_offsetX; x < m_offsetX + CHUNK_WIDTH; x++)
			GenerateBorderColumnMesh(x, z);

	for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++)
		for (int x : {m_offsetX, m_offsetX + CHUNK_WIDTH - 1})
			GenerateBorderColumnMesh(x, z);
}

void Chunk::InitializeBuffers()
{
	m_polygonCount = m_mesh.size() / 6;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_mesh.size(), m_mesh.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

bool Chunk::CanBeUnloaded()
{
	return !(loadingState == LoadingState::loading_blocks || loadingState == LoadingState::generating_mesh);
}

void Chunk::Clear()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	m_VAO = -1;
	m_VBO = -1;
	m_polygonCount = 0;

	FOREACH_CARDINAL_DIRECTION(auto direction, {
		adjacentChunks[direction] = nullptr;
		});

	loadingState = LoadingState::loading_blocks;
}

void Chunk::Render() const
{
	if (loadingState == LoadingState::completed) {
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, m_polygonCount);
	}
}

inline void Chunk::GenerateColumnMesh(int x, int z)
{
	for (int y = 0; y <= 101; y++) {
		BlockPos pos = { x,y,z };
		Block* block = GetBlock(pos);
		if (block->IsOpaque())
			continue;

		FOREACH_DIRECTION(constexpr auto direction,
			{
				Block * neighbour = GetBlock(pos.Adjacent<direction>());

				if (neighbour->IsOpaque())
					neighbour->WriteFace<Direction::Opposite<direction>()>(m_mesh, pos.Adjacent<direction>());
			});
	}
}

inline void Chunk::GenerateBorderColumnMesh(int x, int z)
{
	for (int y = 0; y <= 101; y++) {
		BlockPos pos = { x,y,z };
		Block* block = GetBlock(pos);
		if (block->IsOpaque())
			continue;

		FOREACH_DIRECTION(constexpr auto direction,
			{
				Block * neighbour;
				if constexpr (Direction::IsCardinal<direction>())
					neighbour = GetNearbyBlock<direction>(pos.Adjacent<direction>());
				else
					neighbour = GetBlock(pos.Adjacent<direction>());

				if (neighbour->IsOpaque())
					neighbour->WriteFace<Direction::Opposite<direction>()>(m_mesh, pos.Adjacent<direction>());
			});
	}
}
