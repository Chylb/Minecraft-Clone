#pragma once

#include <vector>
#include <functional>

#include "BakedModel.h"
#include "UnbakedModel.h"
#include "../world/block/Block.h"

class BlockModelRegistry
{
public:
	static void Register(Block* block, std::function<UnbakedModel(const BlockState& state)> mapper);
	static const BakedModel& GetBlockModel(uint16_t id);
	static const BakedModel& GetBlockModel(BlockState* state);

	friend int main();

private:
	inline static std::vector<BakedModel> s_models;

	static void Resize(int size);
};