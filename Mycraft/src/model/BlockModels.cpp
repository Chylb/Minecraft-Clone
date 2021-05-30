#include "BlockModels.h"

#include "BlockModelRegistry.h"
#include "../world/block/Blocks.h"

void BlockModels::Initialize()
{
	Register(Blocks::stone, CubeAll().SetTexture("all", "stone"));
	Register(Blocks::grass, CubeTopBottom().SetTexture("top", "grass_top").SetTexture("side", "grass_block_side").SetTexture("bottom", "dirt"));
	Register(Blocks::dirt, CubeAll().SetTexture("all", "dirt"));
	Register(Blocks::cobblestone, CubeAll().SetTexture("all", "cobblestone"));
	Register(Blocks::plank, CubeAll().SetTexture("all", "oak_planks"));
	Register(Blocks::leaves, CubeAll().SetTexture("all", "leaves"));

	Register(Blocks::wood,
		[](BlockState state) {
			UnbakedModel model = BlockModels::CubeColumn().SetTexture("side", "oak_log").SetTexture("end", "oak_log_top");

			if (state.GetValue(BlockStateProperties::axis) == Axis::x)
				return model.RotateX(90);
			else if (state.GetValue(BlockStateProperties::axis) == Axis::z)
				return model.RotateZ(90);
			return model;
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
	UnbakedQuad north{
		{0,0,0},{0,0},
		{0,1,0},{0,1},
		{1,1,0},{1,1},
		{1,0,0},{1,0},
		"south", Direction::north };

	UnbakedQuad east{
		{1,1,1},{0,1},
		{1,0,1},{0,0},
		{1,0,0},{1,0},
		{1,1,0},{1,1},
		"east", Direction::east };

	UnbakedQuad south{
		{0,0,1},{0,0},
		{1,0,1},{1,0},
		{1,1,1},{1,1},
		{0,1,1},{0,1},
		"south", Direction::south };

	UnbakedQuad west{
		{0,1,1},{1,1},
		{0,1,0},{0,1},
		{0,0,0},{0,0},
		{0,0,1},{1,0},
		"west", Direction::west };

	UnbakedQuad top{
		{0,1,0},{0,0},
		{0,1,1},{0,1},
		{1,1,1},{1,1},
		{1,1,0},{1,0},
		"top", Direction::top };

	UnbakedQuad bottom{
		{0,0,0},{0,0},
		{1,0,0},{1,0},
		{1,0,1},{1,1},
		{0,0,1},{0,1},
		"bottom", Direction::bottom };

	UnbakedModel cube;
	cube.AddQuad(north);
	cube.AddQuad(east);
	cube.AddQuad(south);
	cube.AddQuad(west);
	cube.AddQuad(top);
	cube.AddQuad(bottom);

	return cube.Scale({ 16,16,16 });
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