#include "Blocks.h"

#include "Air.h"
#include "Dirt.h"
#include "Log.h"
#include "Slab.h"

#define TIX(X) Resources::GetTextureIx(X)

void Blocks::Initialize()
{
	air			= BlockRegistry::Register(new Air());
	stone		= BlockRegistry::Register(new Block(true));
	grass		= BlockRegistry::Register(new Block(true));
	dirt		= BlockRegistry::Register(new Dirt());
	cobblestone = BlockRegistry::Register(new Block(true));
	plank		= BlockRegistry::Register(new Block(true));
	wood		= BlockRegistry::Register(new Log());
	leaves		= BlockRegistry::Register(new Block(true));
	slab		= BlockRegistry::Register(new Slab());
}

BlockState* Blocks::GetBlockState(uint16_t id)
{
	return BlockRegistry::GetBlockState(id);
}

