#include "WorldGenerator.h"

#include <numbers>

WorldGenerator::WorldGenerator(World* world)
	: m_world(world)
{
}

void WorldGenerator::GenerateChunk(Chunk& chunk) const
{
	int offsetX = chunk.GetPos().x * Chunk::CHUNK_WIDTH;
	int offsetZ = chunk.GetPos().z * Chunk::CHUNK_WIDTH;

	for (int x = 0; x < Chunk::CHUNK_WIDTH; x++)
		for (int z = 0; z < Chunk::CHUNK_WIDTH; z++) {
			int h = 80 + 10 * sin((x + offsetX) * std::numbers::pi / Chunk::CHUNK_WIDTH) + 10 * sin((z + offsetZ) * std::numbers::pi / Chunk::CHUNK_WIDTH);

			uint16_t blockId = Blocks::stone->GetId();
			for (int y = 0; y < h - 5; y++) {
				chunk.LocalSetBlock({ x,y,z }, blockId);
			}

			blockId = Blocks::dirt->GetId();
			for (int y = h - 5; y < h; y++) {
				chunk.LocalSetBlock({ x,y,z }, blockId);
			}

			blockId = Blocks::grass->GetId();
			chunk.LocalSetBlock({ x,h,z }, blockId);

			for (int y = h + 1; y < Chunk::CHUNK_HEIGHT; y++) {
				chunk.LocalSetBlock({ x,y,z }, 0);
			}
		}

	for (int i = 0; i < 8; i++) {
		chunk.LocalSetBlock({ i,250,0 }, i);
	}
}