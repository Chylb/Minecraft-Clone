#include "Blocks.h"

#include "Air.h"
#include "Dirt.h"
#include "Flower.h"
#include "Log.h"
#include "Slab.h"
#include "TrapDoor.h"

void Blocks::Initialize()
{
	air			= BlockRegistry::Register(new Air());
	stone		= BlockRegistry::Register(new Block());
	grass		= BlockRegistry::Register(new Block());
	dirt		= BlockRegistry::Register(new Dirt());
	cobblestone = BlockRegistry::Register(new Block());
	plank		= BlockRegistry::Register(new Block());
	wood		= BlockRegistry::Register(new Log());
	leaves		= BlockRegistry::Register(new Block());
	slab		= BlockRegistry::Register(new Slab());
	flower		= BlockRegistry::Register(new Flower());
	debugBlock	= BlockRegistry::Register(new Flower());
	trapDoor	= BlockRegistry::Register(new TrapDoor());
}

BlockState* Blocks::GetBlockState(uint16_t id)
{
	return BlockRegistry::GetBlockState(id);
}

