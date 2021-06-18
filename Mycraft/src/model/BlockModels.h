#pragma once

#include <functional>

#include "UnbakedModel.h"
#include "../world/block/BlockState.h"

namespace BlockModels {
	void Initialize();

	void Register(Block* block, std::function<UnbakedModel(BlockState)> mapper);
	void Register(Block* block, UnbakedModel model);

	UnbakedModel Cube();
	UnbakedModel CubeAll();
	UnbakedModel CubeColumn();
	UnbakedModel CubeTopBottom();

	UnbakedModel MissingModel();

	UnbakedModel Cross();
	UnbakedModel Torch();
	UnbakedModel WallTorch();
};
