#include "BlockRegistry.h"

Block* BlockRegistry::Register(Block* block)
{
	static uint16_t blockStateId = 0;

	s_blocks.push_back(block);

	const auto& states = block->GetStateDefinition().GetPossibleStates();
	for (auto state : states) {
		state->m_id = blockStateId;
		s_blockStates.push_back(state);
		blockStateId++;
	}

	return block;
}

BlockState* BlockRegistry::GetBlockState(uint16_t id)
{
	return s_blockStates[id];
}
