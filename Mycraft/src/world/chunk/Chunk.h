#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "../block/Block.h"
#include "../block/Blocks.h"
#include "../block/BlockPos.h"
#include "../chunk/ChunkPos.h"
class World;

class Chunk
{
public:
	Chunk();
	~Chunk();

	Block* LocalGetBlock(BlockPos pos) const;
	Block* SafeLocalGetBlock(BlockPos pos) const;
	Block* GetBlock(BlockPos pos) const;
	void LocalSetBlock(BlockPos pos, uint16_t blockId);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void SetWorld(World* world);
	ChunkPos GetPos() const;
	void SetPos(int x, int z);

	void GenerateMesh();
	void InitializeBuffers();
	void Clear();
	void Render() const;

	static const int CHUNK_HEIGHT = 256;
	static const int CHUNK_WIDTH = 32;

private:
	World* m_world;
	int m_x, m_z;
	int m_offsetX, m_offsetZ;

	uint16_t m_data[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

	std::vector<float> m_mesh;
	GLuint m_VBO, m_VAO;
	unsigned int m_polygonCount;
	bool m_ready;
};
