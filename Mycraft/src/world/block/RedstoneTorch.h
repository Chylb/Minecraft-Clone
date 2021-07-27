#pragma once

#include "Torch.h"

class RedstoneTorch : public Torch
{
public:
	inline static const auto& lit = BlockStateProperties::lit;

	RedstoneTorch();

	void OnPlace(const BlockState& state, World& world, BlockPos pos, const BlockState& oldState) const override;
	void OnRemove(const BlockState& state, World& world, BlockPos pos, const BlockState& newState) const override;
	void NeighborChanged(const BlockState& state, World& world, BlockPos pos, const Block& updaterBlock, BlockPos updaterPos) const override;
	void Tick(const BlockState& state, World& world, BlockPos pos) const override;
	int GetSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const override;
	int GetDirectSignal(const BlockState& state, const World& world, BlockPos pos, Direction dir) const override;
	bool IsSignalSource(const BlockState& state) const override;

protected:
	virtual bool HasNeighborSignal(const World& world, BlockPos pos, const BlockState& state) const;
};