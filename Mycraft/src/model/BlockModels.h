#pragma once

#include <functional>

#include "UnbakedModel.h"
#include "../world/block/BlockState.h"

namespace BlockModels {
	void Initialize();

	void Register(Block* block, std::function<UnbakedModel(const BlockState& state)> mapper);
	void Register(Block* block, UnbakedModel model);

	void RegisterTorch(Block* torch, std::function<std::string(const BlockState& state)> torchTexture, Block* wallTorch, std::function<std::string(const BlockState& state)> wallTorchTexture);

	UnbakedModel Cube();
	UnbakedModel CubeAll();
	UnbakedModel CubeColumn();
	UnbakedModel CubeTopBottom();

	UnbakedModel MissingModel();

	UnbakedModel Cross();
	UnbakedModel Torch();
	UnbakedModel WallTorch();

	UnbakedModel Grass();

	UnbakedModel DoorBottom();
	UnbakedModel DoorBottomRH();
	UnbakedModel DoorTop();
	UnbakedModel DoorTopRH();

	UnbakedModel RedstoneDot();
	UnbakedModel RedstoneDustSide();
	UnbakedModel RedstoneDustSideAlt();
	UnbakedModel RedstoneDustUp();
	UnbakedModel Tinted();
};
