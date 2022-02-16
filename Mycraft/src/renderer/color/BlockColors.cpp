#include "BlockColors.h"

#include "../../model/BlockModelRegistry.h"
#include "../../model/BlockModels.h"
#include "../../world/block/Blocks.h"
#include "../../world/block/RedstoneWire.h"

void BlockColors::Initialize()
{
	Register([](const BlockState& state, BlockPos pos, int ix)
		{
			float green = 2 / 3.0 + 1 / 3.0 * (pos.y - 60) / 40.0;
			return glm::vec3{ 0,green,0 };
		}, Blocks::grass);

	Register([](const BlockState& state, BlockPos pos, int ix)
		{
			return ((RedstoneWire*)Blocks::redstoneWire)->Color(state.GetValue(RedstoneWire::power));
			//return glm::vec3{ 1,0,0 };
		}, Blocks::redstoneWire);

	Register([](const BlockState& state, BlockPos pos, int ix)
		{
			return glm::vec3{ 63 / 255.0, 118 / 255.0, 228 / 255.0 };
		}, Blocks::water);
}

void BlockColors::Register(std::function<glm::vec3(const BlockState& state, BlockPos pos, int ix)> colorFunc, const Block* block)
{
	for (auto& state : block->GetStateDefinition().GetPossibleStates())
	{
		BakedModel& model = const_cast<BakedModel&>(BlockModelRegistry::GetBlockModel(state));
		model.m_colorFunc = colorFunc;
	}
}