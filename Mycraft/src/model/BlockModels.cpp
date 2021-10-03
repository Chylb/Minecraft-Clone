#include "BlockModels.h"

#include "BlockModelRegistry.h"
#include "../world/block/Blocks.h"
#include "../state/properties/BlockStateProperties.h"
#include "../world/block/RedstoneWire.h"

void BlockModels::Initialize()
{
	Register(Blocks::air, UnbakedModel());
	Register(Blocks::stone, CubeAll().SetTexture("all", "stone"));
	Register(Blocks::dirt, CubeAll().SetTexture("all", "dirt"));
	Register(Blocks::cobblestone, CubeAll().SetTexture("all", "cobblestone"));
	Register(Blocks::plank, CubeAll().SetTexture("all", "oak_planks"));
	Register(Blocks::leaves, CubeAll().SetTexture("all", "leaves"));
	Register(Blocks::flower, Cross().SetTexture("cross", "poppy"));
	Register(Blocks::debugBlock, CubeAll().Scale({ 0.5,0.5,0.5 }).Translate({ 4,4,4 }).SetTexture("all", "debug"));
	Register(Blocks::grass, Grass().SetTexture("top", "grass_block_top").SetTexture("side", "grass_block_side").SetTexture("bottom", "dirt").SetTexture("overlay", "grass_block_side_overlay"));
	Register(Blocks::water, CubeAll().SetTexture("all", "water"));

	RegisterTorch(Blocks::torch,
		[](const BlockState& state) {return "torch"; },
		Blocks::wallTorch,
		[](const BlockState& state) {return "torch"; }
	);

	RegisterTorch(
		Blocks::redstoneTorch,
		[](const BlockState& state) { return state.GetValue(BlockStateProperties::lit) ? "redstone_torch" : "redstone_torch_off"; },
		Blocks::redstoneWallTorch,
		[](const BlockState& state) { return state.GetValue(BlockStateProperties::lit) ? "redstone_torch" : "redstone_torch_off"; }
	);

	Register(Blocks::redstoneWire,
		[](const BlockState& state) {
			if (RedstoneWire::IsDot(state))
				return RedstoneDot().SetTexture("line", "redstone_dust_dot").SetTexture("overlay", "redstone_dust_overlay");

			UnbakedModel model{};

			RedstoneSide sides[] = { state.GetValue(BlockStateProperties::northRedstone), state.GetValue(BlockStateProperties::westRedstone), state.GetValue(BlockStateProperties::southRedstone) , state.GetValue(BlockStateProperties::eastRedstone) };
			float rotation[] = { 0,90,180,270 };

			for (int i = 0; i < 4; i++)
			{
				auto side = sides[i];
				auto rot = rotation[i];

				switch (side)
				{
				case RedstoneSide::side:
					model = model.AddModel(RedstoneDustSide().RotateY(rot));
					break;
				case RedstoneSide::up:
					model = model.AddModel(RedstoneDustSide().RotateY(rot));
					model = model.AddModel(RedstoneDustUp().RotateY(rot));
					break;
				}
			}

			model.SetTexture("line", "redstone_dust_line0");
			model.SetTexture("overlay", "redstone_dust_overlay");

			return model;
		});

	Register(Blocks::door,
		[](const BlockState& state) {
			bool upper = state.GetValue(BlockStateProperties::doubleBlockHalf) == DoubleBlockHalf::upper;
			bool left = state.GetValue(BlockStateProperties::doorHinge) == DoorHingeSide::left;
			UnbakedModel modelLH = upper ? DoorTop() : DoorBottom();
			UnbakedModel modelRH = upper ? DoorTopRH() : DoorBottomRH();
			UnbakedModel modelClosed = left ? modelLH : modelRH;
			UnbakedModel modelOpened = left ? modelClosed.RotateY(90, { 1.5,8,1.5 }, true) : modelClosed.RotateY(-90, { 1.5,8,14.5 }, true);
			UnbakedModel model = state.GetValue(BlockStateProperties::open) ? modelOpened : modelClosed;
			model.SetTexture("top", "oak_door_top");
			model.SetTexture("bottom", "oak_door_bottom");

			switch (state.GetValue(BlockStateProperties::horizontalFacing))
			{
			case Direction::north:
				return model.RotateY(90);
			case Direction::west:
				return model.RotateY(180);
			case Direction::south:
				return model.RotateY(270);
			case Direction::east:
				return model;
			}
			return MissingModel();
		});

	Register(Blocks::wood,
		[](const BlockState& state) {
			UnbakedModel model = BlockModels::CubeColumn().SetTexture("side", "oak_log").SetTexture("end", "oak_log_top");

			if (state.GetValue(BlockStateProperties::axis) == Direction::Axis::x)
				return model.RotateZ(90);
			if (state.GetValue(BlockStateProperties::axis) == Direction::Axis::z)
				return model.RotateX(90);
			return model;
		});

	Register(Blocks::slab,
		[](const BlockState& state) {
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

	Register(Blocks::trapDoor,
		[](const BlockState& state)
		{
			UnbakedModel bottom({ 0, 0, 0 }, { 16,3,16 }, {
				{Direction::down,	{0,0,16,16}, "oak_trapdoor"},
				{Direction::up,	{0,0,16,16}, "oak_trapdoor",1,1},
				{Direction::north,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::south,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::west,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::east,	{0,16,16,13}, "oak_trapdoor"}
				});

			UnbakedModel top({ 0, 13, 0 }, { 16,16,16 }, {
				{Direction::down,	{0,0,16,16}, "oak_trapdoor",1,1},
				{Direction::up,	{0,0,16,16}, "oak_trapdoor"},
				{Direction::north,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::south,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::west,	{0,16,16,13}, "oak_trapdoor"},
				{Direction::east,	{0,16,16,13}, "oak_trapdoor"}
				});

			UnbakedModel open = top.RotateX(-90, { 8,16,16 }).Translate({ 0,0,-3 });

			UnbakedModel model;

			if (!state.GetValue(BlockStateProperties::open)) {
				if (state.GetValue(BlockStateProperties::half) == Half::bottom)
					model = bottom;
				else
					model = top;
			}
			else
				model = open;

			switch (state.GetValue(BlockStateProperties::horizontalFacing))
			{
			case Direction::north:
				return model;
			case Direction::west:
				return model.RotateY(90);
			case Direction::south:
				return model.RotateY(180);
			case Direction::east:
				return model.RotateY(270);
			}
			return MissingModel();
		});

	Register(Blocks::powerSensor,
		[](const BlockState& state)
		{
			UnbakedModel model = UnbakedModel({ 4, 4, 4 }, { 12,12,12 }, {
				{Direction::down,	{0,0,16,16}, "bottom",1,1},
				{Direction::up,		{0,0,16,16}, "top",1,1},
				{Direction::north,	{0,0,16,16}, "north",1,1},
				{Direction::south,	{0,0,16,16}, "south",1,1},
				{Direction::west,	{0,0,16,16}, "west",1,1},
				{Direction::east,	{0,0,16,16}, "east",1,1}
				});

			model.SetTexture("top", state.GetValue(BlockStateProperties::poweredUp) ? "sensor_on" : "sensor_off");
			model.SetTexture("bottom", state.GetValue(BlockStateProperties::poweredDown) ? "sensor_on" : "sensor_off");
			model.SetTexture("north", state.GetValue(BlockStateProperties::poweredNorth) ? "sensor_on" : "sensor_off");
			model.SetTexture("east", state.GetValue(BlockStateProperties::poweredEast) ? "sensor_on" : "sensor_off");
			model.SetTexture("south", state.GetValue(BlockStateProperties::poweredSouth) ? "sensor_on" : "sensor_off");
			model.SetTexture("west", state.GetValue(BlockStateProperties::poweredWest) ? "sensor_on" : "sensor_off");

			return model;
		});
}

void BlockModels::Register(Block* block, std::function<UnbakedModel(const BlockState& state)> mapper)
{
	BlockModelRegistry::Register(block, mapper);
}

void BlockModels::Register(Block* block, UnbakedModel model)
{
	BlockModelRegistry::Register(block, [&](const BlockState& state) {return model; });
}

void BlockModels::RegisterTorch(Block* torch, std::function<std::string(const BlockState& state)> torchTexture, Block* wallTorch, std::function<std::string(const BlockState& state)> wallTorchTexture)
{
	Register(torch,
		[&](const BlockState& state) {
			auto model = Torch();
			model.SetTexture("torch", torchTexture(state));
			return model;
		});

	Register(wallTorch,
		[&](const BlockState& state) {
			UnbakedModel model = WallTorch();
			switch (state.GetValue(BlockStateProperties::horizontalFacing))
			{
			case Direction::north:
				break;
			case Direction::west:
				model = model.RotateY(90);
				break;
			case Direction::south:
				model = model.RotateY(180);
				break;
			case Direction::east:
				model = model.RotateY(270);
				break;
			default:
				return MissingModel();
			}
			model.SetTexture("torch", wallTorchTexture(state));
			return model;
		});
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

UnbakedModel BlockModels::MissingModel()
{
	return CubeAll().SetTexture("all", "missing_texture");
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

UnbakedModel BlockModels::Torch()
{
	auto planesY = UnbakedModel({ 7, 0, 7 }, { 9,10,9 }, {
				{Direction::down,	{7, 13, 9, 15}, "torch",1,1},
				{Direction::up,	{7,  6, 9,  8}, "torch",1,1},
		});

	auto planesX = UnbakedModel({ 7, 0, 0 }, { 9, 16, 16 }, {
				{Direction::west,	{0,0,16,16}, "torch",1,1},
				{Direction::east,	{0,0,16,16}, "torch",1,1},
		});

	auto planesZ = UnbakedModel({ 0, 0, 7 }, { 16, 16, 9 }, {
				{Direction::north,	{0,0,16,16}, "torch",1,1},
				{Direction::south,	{0,0,16,16}, "torch",1,1},
		});

	return planesX.AddModel(planesY).AddModel(planesZ);
}

UnbakedModel BlockModels::WallTorch()
{
	return Torch().Translate({ 0,3.5,-8 }).RotateX(22.5, { 8,3.5,0 }).RotateY(180);
}

UnbakedModel BlockModels::Grass()
{
	return UnbakedModel({ 0, 0, 0 }, { 16,16,16 }, {
			{Direction::north,	{0,0,16,16}, "overlay",1,0,0},
			{Direction::south,	{0,0,16,16}, "overlay",1,0,0},
			{Direction::west,	{0,0,16,16}, "overlay",1,0,0},
			{Direction::east,	{0,0,16,16}, "overlay",1,0,0},

			{Direction::down,	{0,0,16,16}, "bottom"},
			{Direction::up,	{0,0,16,16}, "top",1,0,0},
			{Direction::north,	{0,0,16,16}, "side"},
			{Direction::south,	{0,0,16,16}, "side"},
			{Direction::west,	{0,0,16,16}, "side"},
			{Direction::east,	{0,0,16,16}, "side"},
		});
}

UnbakedModel BlockModels::DoorBottom()
{
	return UnbakedModel({ 0, 0, 0 }, { 3, 16, 16 }, {
				{Direction::down,	{13, 0, 16, 16}, "bottom"},
				{Direction::north,	{ 3, 0,  0, 16}, "bottom"},
				{Direction::south,	{ 0, 0,  3, 16}, "bottom"},
				{Direction::west,	{ 0, 0, 16, 16}, "bottom"},
				{Direction::east,	{16, 0,  0, 16}, "bottom",1,1}
		});
}

UnbakedModel BlockModels::DoorBottomRH()
{
	return UnbakedModel({ 0, 0, 0 }, { 3, 16, 16 }, {
				{Direction::down,	{13, 0, 16, 16}, "bottom"},
				{Direction::north,	{ 3, 0,  0, 16}, "bottom"},
				{Direction::south,	{ 0, 0,  3, 16}, "bottom"},
				{Direction::west,	{16, 0,  0, 16}, "bottom"},
				{Direction::east,	{ 0, 0, 16, 16}, "bottom",1,1}
		});
}

UnbakedModel BlockModels::DoorTop()
{
	return UnbakedModel({ 0, 0, 0 }, { 3, 16, 16 }, {
				{Direction::up,		{13, 0, 16, 16}, "bottom"},
				{Direction::north,	{ 3, 0,  0, 16}, "top"},
				{Direction::south,	{ 0, 0,  3, 16}, "top"},
				{Direction::west,	{ 0, 0, 16, 16}, "top"},
				{Direction::east,	{16, 0,  0, 16}, "top",1,1}
		});
}

UnbakedModel BlockModels::DoorTopRH()
{
	return UnbakedModel({ 0, 0, 0 }, { 3, 16, 16 }, {
				{Direction::up,		{13, 0, 16, 16}, "bottom"},
				{Direction::north,	{ 3, 0,  0, 16}, "top"},
				{Direction::south,	{ 0, 0,  3, 16}, "top"},
				{Direction::west,	{16, 0,  0, 16}, "top"},
				{Direction::east,	{ 0, 0, 16, 16}, "top",1,1}
		});
}

UnbakedModel BlockModels::RedstoneDot()
{
	return UnbakedModel({ 0, 0.25, 0 }, { 16, 0.25, 16 }, {
				{Direction::up,		{0, 0, 16, 16}, "line",1,1,0},
				{Direction::down,	{0, 16, 16, 0}, "line",1,1,0},

				{Direction::up,		{0, 0, 16, 16}, "overlay",1,1},
				{Direction::down,	{0, 16, 16, 0}, "overlay",1,1},
		});
}

UnbakedModel BlockModels::RedstoneDustSide()
{
	return UnbakedModel({ 0, 0.25, 0 }, { 16, 0.25, 8 }, {
				{Direction::up,		{0, 0, 16, 8}, "line",1,1, 0},
				{Direction::down,	{0, 8, 16, 0}, "line",1,1, 0},

				{Direction::up,		{0, 0, 16, 8}, "overlay",1,1},
				{Direction::down,	{0, 8, 16, 0}, "overlay",1,1},
		});
}

UnbakedModel BlockModels::RedstoneDustSideAlt()
{
	return UnbakedModel({ 0, 0.25, 8 }, { 16, 0.25, 16 }, {
				{Direction::up,		{0, 8, 16, 16}, "line",1,1},
				{Direction::down,	{0, 16, 16, 8}, "line",1,1},
		});
}

UnbakedModel BlockModels::RedstoneDustUp()
{
	return UnbakedModel({ 0, 0, 0.25 }, { 16, 16, 0.25 }, {
				{Direction::south,	{0, 0, 16, 16}, "line",1,1, 0},
				{Direction::north,	{16, 0, 0, 16}, "line",1,1, 0},

				{Direction::south,	{0, 0, 16, 16}, "overlay",1,1},
				{Direction::north,	{16, 0, 0, 16}, "overlay",1,1},
		});
}
