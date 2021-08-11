#include "BlockRegistry.h"

Block* BlockRegistry::Register(Block* block)
{
	static uint16_t blockStateId = 0;

	s_blocks.push_back(block);

	const auto& states = block->GetStateDefinition().GetPossibleStates();
	for (auto state : states) {
		state->m_id = blockStateId;
		state->FillCacheFields();
		s_blockStates.push_back(state);
		blockStateId++;
	}

	return block;
}

BlockState* BlockRegistry::GetBlockState(uint16_t id)
{
	return s_blockStates.at(id);
}

int BlockRegistry::GetBlockStateCount()
{
	return s_blockStates.size();
}

const std::vector<Block*>& BlockRegistry::GetBlocks()
{
	return s_blocks;
}
