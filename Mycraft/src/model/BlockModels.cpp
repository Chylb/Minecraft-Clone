#include "BlockModels.h"

#include "BlockModelRegistry.h"
#include "../world/block/Blocks.h"
#include "../state/properties/BlockStateProperties.h"

void BlockModels::Initialize()
{
	Register(Blocks::air, UnbakedModel());
	Register(Blocks::stone, CubeAll().SetTexture("all", "stone"));
	Register(Blocks::grass, CubeTopBottom().SetTexture("top", "grass_top").SetTexture("side", "grass_block_side").SetTexture("bottom", "dirt"));
	Register(Blocks::dirt, CubeAll().SetTexture("all", "dirt"));
	Register(Blocks::cobblestone, CubeAll().SetTexture("all", "cobblestone"));
	Register(Blocks::plank, CubeAll().SetTexture("all", "oak_planks"));
	Register(Blocks::leaves, CubeAll().SetTexture("all", "leaves"));
	Register(Blocks::flower, Cross().SetTexture("cross", "poppy"));

	Register(Blocks::wood,
		[](BlockState state) {
			UnbakedModel model = BlockModels::CubeColumn().SetTexture("side", "oak_log").SetTexture("end", "oak_log_top");

			if (state.GetValue(BlockStateProperties::axis) == Direction::Axis::x)
				return model.RotateZ(90);
			if (state.GetValue(BlockStateProperties::axis) == Direction::Axis::z)
				return model.RotateX(90);
			return model;
		});

	Register(Blocks::slab,
		[](BlockState state) {
			if (state.GetValue(BlockStateProperties::slabType) == SlabType::bottom)
				return UnbakedModel({ 0, 0, 0 }, { 16,8,16 }, {
				{Direction::down,	{0,0,16,16}, "oak_planks"},
				{Direction::up,	{0,0,16,16}, "oak_planks", 1, 1},
				{Direction::north,	{0,8,16,16}, "oak_planks"},
				{Direction::south,	{0,8,16,16}, "oak_planks"},
				{Direction::west,	{0,8,16,16}, "oak_planks"},
				{Direction::east,	{0,8,16,16}, "oak_planks"}
					});

			if (state.GetValue(BlockStateProperties::slabType) == SlabType::top)
				return UnbakedModel({ 0, 8, 0 }, { 16,16,16 }, {
				{Direction::down,	{0,0,16,16}, "oak_planks",1,1},
				{Direction::up,	{0,0,16,16}, "oak_planks"},
				{Direction::north,	{0,0,16,8}, "oak_planks"},
				{Direction::south,	{0,0,16,8}, "oak_planks"},
				{Direction::west,	{0,0,16,8}, "oak_planks"},
				{Direction::east,	{0,0,16,8}, "oak_planks"}
					});


			return CubeAll().SetTexture("all", "oak_planks");
		});
}

void BlockModels::Register(Block* block, std::function<UnbakedModel(BlockState)> mapper)
{
	BlockModelRegistry::Register(block, mapper);
}

void BlockModels::Register(Block* block, UnbakedModel model)
{
	BlockModelRegistry::Register(block, [&](BlockState state) {return model; });
}

UnbakedModel BlockModels::Cube()
{
	return UnbakedModel({ 0, 0, 0 }, { 16,16,16 }, {
				{Direction::down,	{0,0,16,16}, "bottom"},
				{Direction::up,	{0,0,16,16}, "top"},
				{Direction::north,	{0,0,16,16}, "north"},
				{Direction::south,	{0,0,16,16}, "south"},
				{Direction::west,	{0,0,16,16}, "west"},
				{Direction::east,	{0,0,16,16}, "east"}
		});
}

UnbakedModel BlockModels::CubeAll()
{
	UnbakedModel cube = Cube();
	for (auto key : { "north", "east", "south", "west", "top", "bottom" })
		cube.SetTexture(key, "all");
	return cube;
}

UnbakedModel BlockModels::CubeColumn()
{
	UnbakedModel cube = Cube();
	for (auto key : { "north", "east", "south", "west" })
		cube.SetTexture(key, "side");
	for (auto key : { "top", "bottom" })
		cube.SetTexture(key, "end");
	return cube;
}

UnbakedModel BlockModels::CubeTopBottom()
{
	UnbakedModel cube = Cube();
	for (auto key : { "north", "east", "south", "west" })
		cube.SetTexture(key, "side");
	return cube;
}

UnbakedModel BlockModels::Cross()
{
	auto plane1 = UnbakedModel({ 0.8, 0, 8 }, { 15.2,16,8 }, {
				{Direction::north,	{0,0,16,16}, "cross",1,1},
				{Direction::south,	{0,0,16,16}, "cross",1,1},
		}).RotateY(45);

	auto plane2 = UnbakedModel({ 8, 0, 0.8 }, { 8, 16, 15.2 }, {
				{Direction::west,	{0,0,16,16}, "cross",1,1},
				{Direction::east,	{0,0,16,16}, "cross",1,1},
		}).RotateY(45);

	return plane1.AddModel(plane2);
}