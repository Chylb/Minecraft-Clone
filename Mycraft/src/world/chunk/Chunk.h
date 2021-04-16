#pragma once

#include <GL/glew.h>

#include <vector>

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

	enum class LoadingState
	{
		loading_blocks,
		loaded_blocks,
		generating_mesh,
		completed
	};

	Block* LocalGetBlock(BlockPos pos) const;
	template<Direction::Direction dir>
	Block* GetNearbyBlock(BlockPos pos) const;
	Block* GetBlock(BlockPos pos) const;
	void LocalSetBlock(BlockPos pos, uint16_t blockId);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void SetWorld(World* world);
	ChunkPos GetPos() const;
	void SetPos(ChunkPos pos);

	bool CanGenerateMesh();
	void GenerateMesh();
	void InitializeBuffers();
	bool CanBeUnloaded();
	void ClearMesh();
	void Clear();
	void Render() const;

	LoadingState loadingState;
	Chunk* adjacentChunks[4];
	unsigned int m_polygonCount;

	static const int CHUNK_HEIGHT = 256;
	static const int CHUNK_WIDTH = 32;

private:
	void GenerateColumnMesh(int x, int z); //can template these functions but dunno how
	void GenerateBorderColumnMesh(int x, int z);

	World* m_world;
	ChunkPos m_pos;
	int m_offsetX, m_offsetZ;

	uint16_t m_data[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

	std::vector<float> m_mesh;
	GLuint m_VBO, m_VAO;
};


