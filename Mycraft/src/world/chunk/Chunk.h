#pragma once

#include <mutex>
#include <shared_mutex>
#include <GL/glew.h>

#include <vector>

#include "ChunkPointers.h"
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
		generated_mesh,
		completed,
		n_states
	};

	Block* LocalGetBlock(BlockPos pos) const;
	template<Direction::Direction dir>
	Block* GetNearbyBlock(BlockPos pos, std::array<ChunkReadPtr, 4>& adjacentChunks) const;
	Block* GetBlock(BlockPos pos) const;
	void LocalSetBlock(BlockPos pos, uint16_t blockId);
	void SetBlock(BlockPos pos, uint16_t blockId);

	void SetWorld(World* world);
	ChunkPos GetPos() const;
	void SetPos(ChunkPos pos);

	//bool CanGenerateMesh();
	void GenerateMesh(std::array<ChunkReadPtr, 4>& adjacentChunks);
	void InitializeBuffers();
	bool CanBeUnloaded();
	void Clear();
	void Render() const;

	//std::mutex writeAccessMutex;
	std::shared_mutex accessMutex;

	std::atomic<LoadingState> loadingState;
	//Chunk* adjacentChunks[4];
	unsigned int m_polygonCount;

	static const int CHUNK_HEIGHT = 256;
	static const int CHUNK_WIDTH = 32;

private:
	void GenerateColumnMesh(int x, int z); //can template these functions but dunno how
	void GenerateBorderColumnMesh(int x, int z, std::array<ChunkReadPtr, 4> &adjacentChunks);

	World* m_world;
	ChunkPos m_pos;
	int m_offsetX, m_offsetZ;
	
	uint16_t m_data[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_HEIGHT];

	std::vector<float> m_mesh;
	GLuint m_VBO, m_VAO;
};

class ChunkWrapper : private Chunk
{
public:
	ChunkReadPtr GetReadPtr(bool blocking);
	ChunkWritePtr GetWritePtr(bool blocking);

	using Chunk::SetWorld;
	using Chunk::GetPos;
	using Chunk::SetPos;
	using Chunk::CanBeUnloaded;
	using Chunk::loadingState;
};

