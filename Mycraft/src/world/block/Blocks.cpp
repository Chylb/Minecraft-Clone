#include "Blocks.h"

#define TIX(X) Resources::GetTextureIx(X)

void Blocks::Initialize()
{
	air			= BlockRegistry::Register(new Block());
	stone		= BlockRegistry::Register(new Block(TIX("stone"), false));
	grass		= BlockRegistry::Register(new Block(TIX("grass_top"), TIX("grass_block_side"), TIX("dirt"), false));
	dirt		= BlockRegistry::Register(new Block(TIX("dirt"), false));
	cobblestone = BlockRegistry::Register(new Block(TIX("cobblestone"), false));
	plank		= BlockRegistry::Register(new Block(TIX("oak_planks"), false));
	wood		= BlockRegistry::Register(new Block(TIX("oak_log_top"), TIX("oak_log"), TIX("oak_log_top"), false));
	leaves		= BlockRegistry::Register(new Block(TIX("leaves"), false));
}

Block* Blocks::GetBlock(uint16_t id)
{
	return BlockRegistry::GetBlock(id);
}

