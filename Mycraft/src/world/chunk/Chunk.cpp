#include "Chunk.h"

#include "../World.h"

Chunk::Chunk() :
	m_ready(false)
{
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
	return { m_x, m_z };
}

void Chunk::SetPos(int x, int z)
{
	m_x = x;
	m_z = z;
	m_offsetX = x * CHUNK_WIDTH;
	m_offsetZ = z * CHUNK_WIDTH;
}

std::vector<float> Chunk::GenerateMesh() const
{
	std::vector<float> vertices;
	vertices.reserve(6144);

	/*for (int x = 0; x < chunk_width; x++)
		for (int z = 0; z < chunk_width; z++)*/

	for (int x = m_offsetX; x < m_offsetX + Chunk::CHUNK_WIDTH; x++)
		for (int z = m_offsetZ; z < m_offsetZ + Chunk::CHUNK_WIDTH; z++)
			for (int y = 0; y < CHUNK_HEIGHT; y++) {
				Block* block = m_world->GetBlock({ x,y,z });
				if (block == Blocks::air)
					continue;

				Block* topBlock = m_world->GetBlock({ x,y + 1,z });
				Block* northBlock = m_world->GetBlock({ x,y,z - 1 });
				Block* eastBlock = m_world->GetBlock({ x + 1,y,z });
				Block* southBlock = m_world->GetBlock({ x,y,z + 1 });
				Block* westBlock = m_world->GetBlock({ x - 1,y,z });
				Block* bottomBlock = m_world->GetBlock({ x,y - 1,z });

				if (topBlock->IsOpaque())
					block->WriteTopFace(vertices, x, y, z);
				if (northBlock->IsOpaque())
					block->WriteNorthFace(vertices, x, y, z);
				if (eastBlock->IsOpaque())
					block->WriteEastFace(vertices, x, y, z);
				if (southBlock->IsOpaque())
					block->WriteSouthFace(vertices, x, y, z);
				if (westBlock->IsOpaque())
					block->WriteWestFace(vertices, x, y, z);
				if (bottomBlock->IsOpaque())
					block->WriteBottomFace(vertices, x, y, z);
			}
	return vertices;
}

void Chunk::InitializeBuffers()
{
	auto vertices = GenerateMesh();
	m_polygonCount = vertices.size() / 6;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	m_ready = true;
}

void Chunk::Render() const
{
	if (!m_ready)
		return;
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_polygonCount);
}
