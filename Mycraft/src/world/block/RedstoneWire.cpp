#include "RedstoneWire.h"

#include "../World.h"
#include "../../item/BlockItemUseContext.h"
#include "../../utils/MathUtils.h"

RedstoneWire::RedstoneWire()
{
	CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder(this).AddProperty(north).AddProperty(east).AddProperty(south).AddProperty(west).AddProperty(power));
	RegisterDefaultState(m_stateDefinition.Any()->SetValue(north, RedstoneSide::none).SetValue(east, RedstoneSide::none).SetValue(south, RedstoneSide::none).SetValue(west, RedstoneSide::none).SetValue(power, 0));

	cross = &m_defaultBlockState->SetValue(north, RedstoneSide::side).SetValue(east, RedstoneSide::side).SetValue(south, RedstoneSide::side).SetValue(west, RedstoneSide::side);

	for (int i = 0; i <= 15; ++i) {
		float f = (float)i / 15.0F;
		float r = f * 0.6F + (f > 0.0F ? 0.4F : 0.3F);
		float g = MathUtils::clamp(f * f * 0.7F - 0.5F, 0.0F, 1.0F);
		float b = MathUtils::clamp(f * f * 0.6F - 0.7F, 0.0F, 1.0F);
		colors[i] = glm::vec3{ r, g, b };
	}
}

const VoxelShape& RedstoneWire::GetShape(const BlockState& state) const
{
	static auto shape = Box(0, 0, 0, 16, 0.25, 16);
	return shape;
}

const BlockState* RedstoneWire::GetStateForPlacement(const BlockItemUseContext& useContext) const
{
	return &GetConnectionState(useContext.GetWorld(), *cross, useContext.GetClickedPos());
}

bool RedstoneWire::CanSurvive(const BlockState& state, const World& world, BlockPos pos) const
{
	return world.GetBlockState(pos.Below())->IsFaceSturdy(world, pos.Below(), Direction::up);
}

const BlockState& RedstoneWire::UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	if (from == Direction::down)
		return oldState;
	if (from = Direction::up)
		return GetConnectionState(world, oldState, pos);

	auto side = GetConnectingSide(world, pos, from);
	return (side != RedstoneSide::none) == (oldState.GetValue(*propertyByDirection.at(from)) != RedstoneSide::none) && !IsCross(oldState) ? oldState.SetValue(*propertyByDirection.at(from), side) : GetConnectionState(world, cross->SetValue(power, oldState.GetValue(power)).SetValue(*propertyByDirection.at(from), side), pos);
}

void RedstoneWire::UpdateIndirectNeighbourShapes(const BlockState& state, World& world, BlockPos pos, int flags) const
{
	BlockPos pos_mutable;

	for (Direction dir : Direction::horizontalDirections)
	{
		auto side = state.GetValue(*propertyByDirection.at(dir));
		if (side != RedstoneSide::none && !world.GetBlockState(pos_mutable.SetWithOffSet(pos, dir))->Is(*this))
		{
			pos_mutable.Move(Direction::down);
			auto& neighbor = *world.GetBlockState(pos_mutable);

			BlockPos pos3 = pos_mutable.Adjacent(dir.GetOpposite());
			auto& neighbor2 = neighbor.UpdateShape(dir.GetOpposite(), *world.GetBlockState(pos3), world, pos_mutable);

			UpdateOrDestroy(neighbor, neighbor2, world, pos_mutable, flags);

			///

			pos_mutable.SetWithOffSet(pos, dir).Move(Direction::up);
			auto& neighbor3 = *world.GetBlockState(pos_mutable);
			BlockPos blockpos1 = pos_mutable.Adjacent(dir.GetOpposite());
			auto& neighbor4 = neighbor3.UpdateShape(dir.GetOpposite(), *world.GetBlockState(blockpos1), world, pos_mutable);

			UpdateOrDestroy(neighbor3, neighbor4, world, pos_mutable, flags);
		}
	}
}

void RedstoneWire::NeighborChanged(const BlockState& state, World& world, BlockPos pos, const Block& updaterBlock, BlockPos updaterPos) const
{
	if (state.CanSurvive(world, pos))
	{
		UpdatePowerStrength(world, pos, state);
	}
	else
	{
		world.SetBlock(pos, *Blocks::air->DefaultBlockState(), 3);
	}
}

void RedstoneWire::OnPlace(const BlockState& state, World& world, BlockPos pos, const BlockState& oldState) const
{
	if (&state.GetBlock() != &oldState.GetBlock())
	{
		UpdatePowerStrength(world, pos, state);

		for (Direction dir : Direction::verticalDirections)
			world.UpdateNeighborsAt(pos.Adjacent(dir), *this);

		UpdateNeighborsOfNeighboringWires(world, pos);
	}
}

void RedstoneWire::OnRemove(const BlockState& state, World& world, BlockPos pos, const BlockState& newState) const
{
	if (&state.GetBlock() != &newState.GetBlock())
	{
		for (Direction dir : Direction::directions)
			world.UpdateNeighborsAt(pos.Adjacent(dir), *this);


		UpdatePowerStrength(world, pos, state);
		UpdateNeighborsOfNeighboringWires(world, pos);
	}
}

int RedstoneWire::GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const
{
	if (m_shouldSignal && dir != Direction::down)
	{
		int sig = state.GetValue(power);
		if (sig == 0)
			return 0;

		return (dir != Direction::up) && !(GetConnectionState(world, state, pos).GetValue(*propertyByDirection.at(dir.GetOpposite())) != RedstoneSide::none) ? 0 : sig;
	}
	return 0;
}

int RedstoneWire::GetDirectSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const
{
	return !m_shouldSignal ? 0 : state.GetSignal(world, pos, dir);
}

bool RedstoneWire::IsSignalSource(const BlockState& state) const
{
	return m_shouldSignal;
}

bool RedstoneWire::OccludesFace(Direction dir, const BlockState& state) const
{
	return false;
}

RedstoneSide RedstoneWire::GetConnectingSide(const World& world, BlockPos pos, Direction dir) const
{
	return GetConnectingSide(world, pos, dir, !world.GetBlockState(pos.Above())->IsRedstoneConductor(world, pos));
}

RedstoneSide RedstoneWire::GetConnectingSide(const World& world, BlockPos pos, Direction dir, bool nonConductorAbove) const
{
	auto neighborPos = pos.Adjacent(dir);
	const auto& neighbor = *world.GetBlockState(neighborPos);

	if (nonConductorAbove) {
		bool canSurvive = CanSurvive(neighbor, world, neighborPos);
		if (canSurvive && CanConnectTo(*world.GetBlockState(neighborPos.Above()), world, neighborPos.Above(), Direction::none))
		{
			if (neighbor.IsFaceSturdy(world, neighborPos, dir.GetOpposite()))
				return RedstoneSide::up;

			return RedstoneSide::side;
		}
	}
	return !CanConnectTo(neighbor, world, neighborPos, dir) && (neighbor.IsRedstoneConductor(world, neighborPos) || !CanConnectTo(*world.GetBlockState(neighborPos.Below()), world, neighborPos.Below(), Direction::none)) ? RedstoneSide::none : RedstoneSide::side;
}

void RedstoneWire::UpdatePowerStrength(World& world, BlockPos pos, const BlockState& state) const
{
	int strength = CalculateTargetStrength(world, pos);
	if (state.GetValue(power) != strength) {
		world.SetBlock(pos, state.SetValue(power, strength), 2);

		world.UpdateNeighborsAt(pos, *this);
		for (auto dir : Direction::directions)
			world.UpdateNeighborsAt(pos.Adjacent(dir), *this);
	}
}

void RedstoneWire::UpdateNeighborsOfNeighboringWires(World& world, BlockPos pos) const
{
	for (Direction dir : Direction::horizontalDirections)
		CheckCornerChangeAt(world, pos.Adjacent(dir));

	for (Direction dir : Direction::horizontalDirections)
	{
		auto relPos = pos.Adjacent(dir);
		if (world.GetBlockState(relPos)->IsRedstoneConductor(world, relPos))
			CheckCornerChangeAt(world, relPos.Above());
		else
			CheckCornerChangeAt(world, relPos.Below());
	}
}

void RedstoneWire::CheckCornerChangeAt(World& world, BlockPos pos) const
{
	if (world.GetBlockState(pos)->Is(*this))
	{
		world.UpdateNeighborsAt(pos, *this);

		for (auto dir : Direction::directions)
		{
			world.UpdateNeighborsAt(pos.Adjacent(dir), *this);
		}
	}
}

int RedstoneWire::CalculateTargetStrength(World& world, BlockPos pos) const
{
	m_shouldSignal = false;
	int sig = world.GetBestNeighborSignal(pos);
	m_shouldSignal = true;

	int sig2 = 0;
	if (sig < 15)
	{
		for (Direction dir : Direction::horizontalDirections)
		{
			auto neighborPos = pos.Adjacent(dir);
			auto& neighbor = *world.GetBlockState(neighborPos);
			sig2 = std::max(sig2, GetWireSignal(neighbor));
			auto abovePos = pos.Above();
			if (neighbor.IsRedstoneConductor(world, neighborPos) && !world.GetBlockState(abovePos)->IsRedstoneConductor(world, abovePos))
				sig2 = std::max(sig2, GetWireSignal(*world.GetBlockState(neighborPos.Above())));
			else
				sig2 = std::max(sig2, GetWireSignal(*world.GetBlockState(neighborPos.Below())));
		}
	}
	return std::max(sig, sig2 - 1);
}

int RedstoneWire::GetWireSignal(const BlockState& state) const
{
	return state.Is(*this) ? state.GetValue(power) : 0;
}

bool RedstoneWire::CanConnectTo(const BlockState& state, const World& world, BlockPos pos, Direction dir)
{
	if (state.Is(*Blocks::redstoneWire)) {
		return true;
	}
	else
	{
		return state.IsSignalSource();
	}
}

bool RedstoneWire::IsDot(const BlockState& state)
{
	for (Direction dir : Direction::horizontalDirections)
	{
		auto& sideProp = *propertyByDirection.at(dir);
		if (state.GetValue(sideProp) != RedstoneSide::none)
			return false;
	}
	return true;
}

bool RedstoneWire::IsCross(const BlockState& state)
{
	for (Direction dir : Direction::horizontalDirections)
	{
		auto& sideProp = *propertyByDirection.at(dir);
		if (state.GetValue(sideProp) == RedstoneSide::none)
			return false;
	}
	return true;
}

glm::vec3 RedstoneWire::Color(int power)
{
	return colors[power];
}

const BlockState& RedstoneWire::GetConnectionState(const World& world, const BlockState& state, BlockPos pos) const
{
	const BlockState* res = &GetMissingConnections(world, m_defaultBlockState->SetValue(power, state.GetValue(power)), pos);

	if (IsDot(*res))
		return *res;

	bool connectedN = res->GetValue(north) != RedstoneSide::none;
	bool connectedS = res->GetValue(south) != RedstoneSide::none;
	bool connectedE = res->GetValue(east) != RedstoneSide::none;
	bool connectedW = res->GetValue(west) != RedstoneSide::none;
	bool notConnectedNS = !connectedN && !connectedS;
	bool notConnectedWE = !connectedE && !connectedW;

	if (!connectedW && notConnectedNS)
		res = &res->SetValue(west, RedstoneSide::side);

	if (!connectedE && notConnectedNS)
		res = &res->SetValue(east, RedstoneSide::side);

	if (!connectedN && notConnectedWE)
		res = &res->SetValue(north, RedstoneSide::side);

	if (!connectedS && notConnectedWE)
		res = &res->SetValue(south, RedstoneSide::side);

	return *res;
}

const BlockState& RedstoneWire::GetMissingConnections(const World& world, const BlockState& state, BlockPos pos) const
{
	const BlockState* res = &state;
	bool nonConductorAbove = !world.GetBlockState(pos.Above())->IsRedstoneConductor(world, pos);

	for (Direction dir : Direction::horizontalDirections)
	{
		if (res->GetValue(*propertyByDirection.at(dir)) == RedstoneSide::none)
		{
			RedstoneSide side = GetConnectingSide(world, pos, dir, nonConductorAbove);
			res = &res->SetValue(*propertyByDirection.at(dir), side);
		}
	}
	return *res;
}


