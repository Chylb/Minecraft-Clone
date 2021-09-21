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

	const BlockState* RawGetBlockState(BlockPos pos) const;
	const BlockState* GetBlockState(BlockPos pos) const;
	void SetBlock(BlockPos pos, const BlockState* state);

	template<Direction dir>
	const BlockState* GetNearbyBlockState(BlockPos pos) const;

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

	void Tick();

	LoadingState loadingState;
	Chunk* adjacentChunks[4];
	unsigned int m_vertexCount;

	bool dirtyMesh;
	int highestBlock = 0;

	static const int CHUNK_HEIGHT = 256;
	static const int CHUNK_WIDTH = 32;

private:
	void GenerateColumnMesh(int x, int z, int h); //can template these functions but dunno how
	void GenerateBorderColumnMesh(int x, int z, int h);

	World* m_world;
	ChunkPos m_pos;
	int m_offsetX, m_offsetZ;

	uint16_t m_data[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

	GLuint m_VBO, m_VAO;
};


