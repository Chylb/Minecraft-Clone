#pragma once

#include "Block.h"

#include <unordered_map>
#include <map>

class RedstoneWire : public Block
{
public:
	inline static const auto& north = BlockStateProperties::northRedstone;
	inline static const auto& east = BlockStateProperties::eastRedstone;
	inline static const auto& south = BlockStateProperties::southRedstone;
	inline static const auto& west = BlockStateProperties::westRedstone;
	inline static const auto& power = BlockStateProperties::power;

	inline static const std::map<Direction, const Property<RedstoneSide>*> propertyByDirection = { {Direction::north, &north}, {Direction::east, &east},{Direction::south, &south},{Direction::west, &west} };
	const BlockState* cross;

	inline static glm::vec3 colors[16];

	RedstoneWire();

	const VoxelShape& GetShape(const BlockState& state) const override;
	const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const override;
	bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const override;
	const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const override;
	void UpdateIndirectNeighbourShapes(const BlockState& state, World& world, BlockPos pos, int flags) const override;
	void NeighborChanged(const BlockState& state, World& world, BlockPos pos, const Block& updaterBlock, BlockPos updaterPos) const override;
	void OnPlace(const BlockState& state, World& world, BlockPos pos, const BlockState& oldState) const override;
	void OnRemove(const BlockState& state, World& world, BlockPos pos, const BlockState& newState) const override;
	int GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const override;
	int GetDirectSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const override;
	bool IsSignalSource(const BlockState& state) const override;

	bool OccludesFace(Direction dir, const BlockState& state) const override;

	static bool IsDot(const BlockState& state);
	static bool IsCross(const BlockState& state);

	static glm::vec3 Color(int power);

private:
	mutable bool m_shouldSignal = true;

	const BlockState& GetConnectionState(const World& world, const BlockState& state, BlockPos pos) const;
	const BlockState& GetMissingConnections(const World& world, const BlockState& state, BlockPos pos) const;
	RedstoneSide GetConnectingSide(const World& world, BlockPos pos, Direction dir) const;
	RedstoneSide GetConnectingSide(const World& world, BlockPos pos, Direction dir, bool nonConductorAbove) const;
	void UpdatePowerStrength(World& world, BlockPos pos, const BlockState& state) const;
	void UpdateNeighborsOfNeighboringWires(World& world, BlockPos pos) const;
	void CheckCornerChangeAt(World& world, BlockPos pos) const;
	int CalculateTargetStrength(World& world, BlockPos pos) const;
	int GetWireSignal(const BlockState& state) const;

	static bool CanConnectTo(const BlockState& state, const World& world, BlockPos pos, Direction dir);
};