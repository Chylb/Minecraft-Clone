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

Block* Chunk::SafeLocalGetBlock(BlockPos pos) const
{
	if (pos.x < 0 || pos.x >= CHUNK_WIDTH || pos.y < 0 || pos.y >= CHUNK_HEIGHT || pos.z < 0 || pos.z >= CHUNK_WIDTH)
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

void Chunk::GenerateMesh()
{
	m_mesh.clear();

	for (int x = 0; x < Chunk::CHUNK_WIDTH; x++)
		for (int z = 0; z < Chunk::CHUNK_WIDTH; z++)
			for (int y = 0; y < CHUNK_HEIGHT; y++) {
				Block* block = SafeLocalGetBlock({ x,y,z });
				if (block == Blocks::air)
					continue;

				Block* topBlock = SafeLocalGetBlock({ x,y + 1,z });
				Block* northBlock = SafeLocalGetBlock({ x,y,z - 1 });
				Block* eastBlock = SafeLocalGetBlock({ x + 1,y,z });
				Block* southBlock = SafeLocalGetBlock({ x,y,z + 1 });
				Block* westBlock = SafeLocalGetBlock({ x - 1,y,z });
				Block* bottomBlock = SafeLocalGetBlock({ x,y - 1,z });

				int gx = x + m_offsetX;
				int gz = z + m_offsetZ;

				if (topBlock->IsOpaque())
					block->WriteTopFace(m_mesh, gx, y, gz);
				if (northBlock->IsOpaque())
					block->WriteNorthFace(m_mesh, gx, y, gz);
				if (eastBlock->IsOpaque())
					block->WriteEastFace(m_mesh, gx, y, gz);
				if (southBlock->IsOpaque())
					block->WriteSouthFace(m_mesh, gx, y, gz);
				if (westBlock->IsOpaque())
					block->WriteWestFace(m_mesh, gx, y, gz);
				if (bottomBlock->IsOpaque())
					block->WriteBottomFace(m_mesh, gx, y, gz);
			}
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

	m_ready = true;
}

void Chunk::Clear()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	m_ready = false;
}

void Chunk::Render() const
{
	if (!m_ready)
		return;
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_polygonCount);
}
