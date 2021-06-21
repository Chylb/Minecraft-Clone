#include "WorldGenerator.h"

#include <numbers>

void addAllBlockStates(Chunk& chunk);

WorldGenerator::WorldGenerator(World* world)
	: m_world(world)
{
}

void WorldGenerator::GenerateChunk(Chunk& chunk) const
{
	int offsetX = chunk.GetPos().x * Chunk::CHUNK_WIDTH;
	int offsetZ = chunk.GetPos().z * Chunk::CHUNK_WIDTH;

	for (int x = offsetX; x < Chunk::CHUNK_WIDTH + offsetX; x++)
		for (int z = offsetZ; z < Chunk::CHUNK_WIDTH + offsetZ; z++) {
			int h = 80 + 10 * sin(x * std::numbers::pi / Chunk::CHUNK_WIDTH) + 10 * sin(z * std::numbers::pi / Chunk::CHUNK_WIDTH);

			for (int y = 0; y < h - 5; y++) {
				chunk.SetBlock({ x,y,z }, Blocks::stone->DefaultBlockState());
			}

			for (int y = h - 5; y < h; y++) {
				chunk.SetBlock({ x,y,z }, Blocks::dirt->DefaultBlockState());
			}

			chunk.SetBlock({ x,h,z }, Blocks::grass->DefaultBlockState());

			for (int y = h + 1; y < Chunk::CHUNK_HEIGHT; y++) {
				chunk.SetBlock({ x,y,z }, Blocks::air->DefaultBlockState());
			}
		}

	//addAllBlockStates(chunk);
}

void addAllBlockStates(Chunk& chunk)
{
	int y = 93;

	for (int id = 0; id < BlockRegistry::GetBlockStateCount(); id++) {
		if (id % 32 == 0)
			y -= 3;

		int x = id % 32;
		chunk.SetBlock({ x,y,16 }, Blocks::GetBlockState(id));
	}
}