#pragma once

#include "../chunk/Chunk.h"

class World;

class WorldGenerator
{
public:
	WorldGenerator(World* world);

	void GenerateChunk(Chunk& chunk) const;

private:
	World* m_world;
};