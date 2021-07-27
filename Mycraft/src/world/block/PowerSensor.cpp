#include "PowerSensor.h"
#include "../World.h"

PowerSensor::PowerSensor()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this)
		.AddProperty(poweredUp)
		.AddProperty(poweredDown)
		.AddProperty(poweredNorth)
		.AddProperty(poweredEast)
		.AddProperty(poweredSouth)
		.AddProperty(poweredWest)
	);
}

const BlockState& PowerSensor::UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	const BlockState* state = m_defaultBlockState;
	printf("======\nUpdateShape\n===========\n");

	#define SignalFunc(x,y) GetSignal(x,y)
//#define SignalFunc(x,y) GetDirectSignal(x,y)

	/*state = &state->SetValue(poweredUp, (bool)world.SignalFunc(pos, Direction::up));
	state = &state->SetValue(poweredDown, (bool)world.SignalFunc(pos, Direction::down));
	state = &state->SetValue(poweredNorth, (bool)world.SignalFunc(pos, Direction::north));
	state = &state->SetValue(poweredEast, (bool)world.SignalFunc(pos, Direction::east));
	state = &state->SetValue(poweredSouth, (bool)world.SignalFunc(pos, Direction::south));
	state = &state->SetValue(poweredWest, (bool)world.SignalFunc(pos, Direction::west));*/

	state = &state->SetValue(poweredUp, (bool)world.SignalFunc(pos.Above(), Direction::up));
	state = &state->SetValue(poweredDown, (bool)world.SignalFunc(pos.Below(), Direction::down));
	state = &state->SetValue(poweredNorth, (bool)world.SignalFunc(pos.North(), Direction::north));
	state = &state->SetValue(poweredEast, (bool)world.SignalFunc(pos.East(), Direction::east));
	state = &state->SetValue(poweredSouth, (bool)world.SignalFunc(pos.South(), Direction::south));
	state = &state->SetValue(poweredWest, (bool)world.SignalFunc(pos.West(), Direction::west));

	return *state;
}

void PowerSensor::SetPlacedBy(World& world, BlockPos pos, const BlockState& state) const
{
	auto& desiredState = UpdateShape(state, Direction::none, state, world, pos);
	if (&desiredState != &state)
	{
		world.SetBlock(pos, desiredState, 3);
	}
}

bool PowerSensor::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}


