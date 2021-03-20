#include "BlockRegistry.h"

Block* BlockRegistry::Register(Block* block)
{
	static uint16_t blockId = 0;

	block->m_id = blockId;
	s_blocks.push_back(block);
	blockId++;

	return block;
}

Block* BlockRegistry::GetBlock(uint16_t id)
{
	return s_blocks[id];
}
