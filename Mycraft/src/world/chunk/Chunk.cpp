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

BlockState* Chunk::GetBlockState(BlockPos pos) const
{
	int x = pos.x & 31, z = pos.z & 31;
	if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
		return Blocks::air->DefaultBlockState();
	return Blocks::GetBlockState(m_data[x][z][pos.y]);
}

void Chunk::SetBlock(BlockPos pos, BlockState* state)
{
	if (state != Blocks::air->DefaultBlockState())
		highestBlock = std::max(highestBlock, pos.y);

	int x = pos.x & 31, z = pos.z & 31;
	m_data[x][z][pos.y] = state->GetId();
}

//Block* Chunk::LocalGetBlock(BlockPos pos) const
//{
//	if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
//		return Blocks::air;
//	return Blocks::GetBlock(m_data[pos.x][pos.z][pos.y]);
//}

template<Direction::Direction dir>
BlockState* Chunk::GetNearbyBlockState(BlockPos pos) const
{
	if constexpr (dir == Direction::north) {
		if (pos.z < m_offsetZ)
			return adjacentChunks[dir]->GetBlockState(pos);
	}
	if constexpr (dir == Direction::east) {
		if (pos.x >= m_offsetX + CHUNK_WIDTH)
			return adjacentChunks[dir]->GetBlockState(pos);
	}
	if constexpr (dir == Direction::south) {
		if (pos.z >= m_offsetZ + CHUNK_WIDTH)
			return adjacentChunks[dir]->GetBlockState(pos);
	}
	if constexpr (dir == Direction::west) {
		if (pos.x < m_offsetX)
			return adjacentChunks[dir]->GetBlockState(pos);
	}

	return GetBlockState(pos);
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
	return adjacentChunks[Direction::north] && adjacentChunks[Direction::east] && adjacentChunks[Direction::south] && adjacentChunks[Direction::west];
}

void Chunk::GenerateMesh()
{
	m_mesh.clear();

	int h = std::max({ highestBlock, adjacentChunks[0]->highestBlock, adjacentChunks[1]->highestBlock, adjacentChunks[2]->highestBlock, adjacentChunks[3]->highestBlock }) + 1;

	for (int x = m_offsetX + 1; x < m_offsetX + CHUNK_WIDTH - 1; x++)
		for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++) {
			GenerateColumnMesh(x, z, h);
		}
	for (int z : {m_offsetZ, m_offsetZ + CHUNK_WIDTH - 1})
		for (int x = m_offsetX; x < m_offsetX + CHUNK_WIDTH; x++)
			GenerateBorderColumnMesh(x, z, h);

	for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++)
		for (int x : {m_offsetX, m_offsetX + CHUNK_WIDTH - 1})
			GenerateBorderColumnMesh(x, z, h);
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

void Chunk::ClearMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	m_VAO = -1;
	m_VBO = -1;
	m_polygonCount = 0;

	loadingState = LoadingState::generating_mesh;
	dirtyMesh = false;
}

void Chunk::Clear()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	m_VAO = -1;
	m_VBO = -1;
	m_polygonCount = 0;
	dirtyMesh = false;

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

void Chunk::Tick()
{
	for (int i = 0; i < 48; i++) {
		int x = rand() % Chunk::CHUNK_WIDTH + m_offsetX;
		int y = rand() % Chunk::CHUNK_HEIGHT;
		int z = rand() % Chunk::CHUNK_WIDTH + m_offsetZ;

		BlockPos pos = { x,y,z };
		auto state = GetBlockState({ x,y,z });
		state->GetBlock().Tick(*m_world, pos);
	}
}

inline void Chunk::GenerateColumnMesh(int x, int z, int h)
{
	for (int y = 0; y <= h; y++) {
		BlockPos pos = { x,y,z };
		BlockState* state = GetBlockState(pos);
		if (state->GetBlock().IsOpaque())
			continue;

		FOREACH_DIRECTION(constexpr auto direction,
			{
				BlockState * neighbour = GetBlockState(pos.Adjacent<direction>());

				if (neighbour->GetBlock().IsOpaque())
					neighbour->GetBlock().WriteFace<Direction::Opposite<direction>()>(m_mesh, pos.Adjacent<direction>());
			});
	}
}

inline void Chunk::GenerateBorderColumnMesh(int x, int z, int h)
{
	for (int y = 0; y <= h; y++) {
		BlockPos pos = { x,y,z };
		BlockState* state = GetBlockState(pos);
		if (state->GetBlock().IsOpaque())
			continue;

		FOREACH_DIRECTION(constexpr auto direction,
			{
				BlockState * neighbour;
				if constexpr (Direction::IsCardinal<direction>())
					neighbour = GetNearbyBlockState<direction>(pos.Adjacent<direction>());
				else
					neighbour = GetBlockState(pos.Adjacent<direction>());

				if (neighbour->GetBlock().IsOpaque())
					neighbour->GetBlock().WriteFace<Direction::Opposite<direction>()>(m_mesh, pos.Adjacent<direction>());
			});
	}
}
