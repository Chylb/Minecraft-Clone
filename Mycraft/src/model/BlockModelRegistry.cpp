#include "BlockModelRegistry.h"

#include "BlockModels.h"
#include "../BlockRegistry.h"

void BlockModelRegistry::Register(Block* block, std::function<UnbakedModel(BlockState)> mapper)
{
	for (auto state : block->GetStateDefinition().GetPossibleStates()) {
		s_models[state->GetId()] = mapper(*state).Bake();
		state->m_model = &GetBlockModel(state);
	}
}

const BakedModel& BlockModelRegistry::GetBlockModel(uint16_t id)
{
	return s_models[id];
}

const BakedModel& BlockModelRegistry::GetBlockModel(BlockState* state)
{
	return s_models[state->GetId()];
}

void BlockModelRegistry::Resize(int size)
{
	s_models.resize(size);

	auto missingModel = BlockModels::CubeAll().SetTexture("all", "missing_texture").Bake();

	for (int i = 0; i < size; i++) {
		s_models[i] = missingModel;
		BlockRegistry::GetBlockState(i)->m_model = &GetBlockModel(BlockRegistry::GetBlockState(i));
	}
}
