#include "Blocks.h"

#define TIX(X) Resources::GetTextureIx(X)

void Blocks::Initialize()
{
	air			= BlockRegistry::Register(new Block());
	stone		= BlockRegistry::Register(new Block(TIX("stone"), true));
	grass		= BlockRegistry::Register(new Block(TIX("grass_top"), TIX("grass_block_side"), TIX("dirt"), true));
	dirt		= BlockRegistry::Register(new Dirt(TIX("dirt")));
	cobblestone = BlockRegistry::Register(new Block(TIX("cobblestone"), true));
	plank		= BlockRegistry::Register(new Block(TIX("oak_planks"), true));
	wood		= BlockRegistry::Register(new Block(TIX("oak_log_top"), TIX("oak_log"), TIX("oak_log_top"), true));
	leaves		= BlockRegistry::Register(new Block(TIX("leaves"), true));
}

Block* Blocks::GetBlock(uint16_t id)
{
	return BlockRegistry::GetBlock(id);
}

