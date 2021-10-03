#include "Chunk.h"

#include "../World.h"
#include "../../model/BlockModelRegistry.h"
#include "../../utils/Direction.h"

extern thread_local std::vector<float> g_mesh[RenderType::count];

Chunk::Chunk()
{
	Clear();
}

Chunk::~Chunk()
{
	for (auto vao : m_VAO) {
		glDeleteVertexArrays(1, &vao);
	}
	for (auto vbo : m_VBO) {
		glDeleteBuffers(1, &vbo);
	}
}

const BlockState* Chunk::RawGetBlockState(BlockPos pos) const
{
	return Blocks::GetBlockState(m_data[pos.x & 31][pos.z & 31][pos.y]);
}

const BlockState* Chunk::GetBlockState(BlockPos pos) const
{
	int x = pos.x & 31, z = pos.z & 31;
	if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
		return Blocks::air->DefaultBlockState();
	return Blocks::GetBlockState(m_data[x][z][pos.y]);
}

void Chunk::SetBlock(BlockPos pos, const BlockState* state)
{
	if (state != Blocks::air->DefaultBlockState())
		highestBlock = std::max(highestBlock, pos.y);

	int x = pos.x & 31, z = pos.z & 31;
	m_data[x][z][pos.y] = state->GetId();
}

template<Direction dir>
const BlockState* Chunk::GetNearbyBlockState(BlockPos pos) const
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
	g_mesh[RenderType::solid].clear();
	g_mesh[RenderType::translucent].clear();

	int h = std::max({ highestBlock, adjacentChunks[0]->highestBlock, adjacentChunks[1]->highestBlock, adjacentChunks[2]->highestBlock, adjacentChunks[3]->highestBlock }) + 1;

	for (int x = m_offsetX + 1; x < m_offsetX + CHUNK_WIDTH - 1; x++)
		for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++)
			GenerateColumnMesh(x, z, h);

	for (int z : {m_offsetZ, m_offsetZ + CHUNK_WIDTH - 1})
		for (int x = m_offsetX; x < m_offsetX + CHUNK_WIDTH; x++)
			GenerateBorderColumnMesh(x, z, h);

	for (int z = m_offsetZ + 1; z < m_offsetZ + CHUNK_WIDTH - 1; z++)
		for (int x : {m_offsetX, m_offsetX + CHUNK_WIDTH - 1})
			GenerateBorderColumnMesh(x, z, h);
}

void Chunk::InitializeBuffers()
{
	for (auto renderType = 0; renderType < RenderType::count; renderType++) {
		auto& mesh = g_mesh[renderType];
		if (mesh.size() == 0)
			continue;

		m_vertexCount[renderType] = mesh.size() / BakedQuad::vertex_size;

		glGenVertexArrays(1, &m_VAO[renderType]);
		glGenBuffers(1, &m_VBO[renderType]);
		glBindVertexArray(m_VAO[renderType]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[renderType]);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
	}
}

bool Chunk::CanBeUnloaded()
{
	return !(loadingState == LoadingState::loading_blocks || loadingState == LoadingState::generating_mesh);
}

void Chunk::ClearMesh()
{
	for (auto renderType = 0; renderType < RenderType::count; renderType++) {
		if (m_VAO[renderType] == -1)
			continue;

		glDeleteVertexArrays(1, &m_VAO[renderType]);
		glDeleteBuffers(1, &m_VBO[renderType]);
		m_VAO[renderType] = -1;
		m_VBO[renderType] = -1;
		m_vertexCount[renderType] = 0;
	}

	loadingState = LoadingState::generating_mesh;
	dirtyMesh = false;
}

void Chunk::Clear()
{
	ClearMesh();

	FOREACH_HORIZONTAL_DIRECTION(auto direction, {
		adjacentChunks[direction] = nullptr;
		});

	loadingState = LoadingState::loading_blocks;
}

void Chunk::Render(RenderType renderType) const
{
	if (loadingState == LoadingState::completed) {
		if (m_VAO[renderType] == -1)
			return;

		glBindVertexArray(m_VAO[renderType]);
		glDrawArrays(GL_TRIANGLES, 0, m_vertexCount[renderType]);
	}
}

unsigned int Chunk::VertexCount() const
{
	unsigned int sum = 0;
	for(int renderType = 0; renderType < RenderType::count; renderType++)
	{
		sum += m_vertexCount[renderType];
	}
	return sum;
}

void Chunk::Tick()
{
	for (int i = 0; i < 48; i++) {
		int x = rand() % Chunk::CHUNK_WIDTH + m_offsetX;
		int y = rand() % Chunk::CHUNK_HEIGHT;
		int z = rand() % Chunk::CHUNK_WIDTH + m_offsetZ;

		BlockPos pos = { x,y,z };
		auto state = GetBlockState({ x,y,z });
		state->RandomTick(*m_world, pos);
	}
}

inline void Chunk::GenerateColumnMesh(int x, int z, int h)
{
	for (int y = 1; y <= h; y++) {
		BlockPos pos = { x,y,z };
		const BlockState* state = RawGetBlockState(pos);

		[[unlikely]]
		if (state->GetModel().HasFace(Direction::none))
			state->GetModel().WriteFace(g_mesh[state->GetRenderType()], *state, pos, Direction::none);

		if (state->OccludesAllFaces())
			continue;

		FOREACH_DIRECTION(constexpr Direction direction,
			{
				if (state->OccludesFace(direction))
					continue;

				const BlockState* neighbour = RawGetBlockState(pos.Adjacent<direction>());

				if (state->GetRenderType() == translucent && state == neighbour)
					continue;

				if (neighbour->GetModel().HasFace(direction.GetOpposite()))
					neighbour->GetModel().WriteFace(g_mesh[neighbour->GetRenderType()], *neighbour, pos.Adjacent<direction>(), direction.GetOpposite());
			})
	}
}

inline void Chunk::GenerateBorderColumnMesh(int x, int z, int h)
{
	for (int y = 1; y <= h; y++) {
		BlockPos pos = { x,y,z };
		const BlockState* state = RawGetBlockState(pos);

		[[unlikely]]
		if (state->GetModel().HasFace(Direction::none))
			state->GetModel().WriteFace(g_mesh[state->GetRenderType()], *state, pos, Direction::none);

		if (state->OccludesAllFaces())
			continue;

		FOREACH_DIRECTION(constexpr Direction direction,
			{
				if (state->OccludesFace(direction))
					continue;

				const BlockState* neighbour;
				if constexpr (direction.IsHorizontal())
					neighbour = GetNearbyBlockState<direction>(pos.Adjacent<direction>());
				else
					neighbour = RawGetBlockState(pos.Adjacent<direction>());

				if (state->GetRenderType() == translucent && state == neighbour)
					continue;

				if (neighbour->GetModel().HasFace(direction.GetOpposite()))
					neighbour->GetModel().WriteFace(g_mesh[neighbour->GetRenderType()], *neighbour, pos.Adjacent<direction>(), direction.GetOpposite());
			})
	}
}
