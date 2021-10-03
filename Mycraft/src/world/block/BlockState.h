#pragma once

#include "../../state/StateHolder.h"
#include "../../utils/Direction.h"
#include "../../renderer/RenderType.h"

class BlockItemUseContext;
struct BlockPos;
struct BlockRayTraceResult;
class BakedModel;
class Block;
class VoxelShape;
class World;

class BlockState : public StateHolder<Block, BlockState>
{
public:
	BlockState() : StateHolder<Block, BlockState>() {}
	//BlockState(const BlockState& state) = delete;

	uint16_t GetId() const;
	const Block& GetBlock() const;

	bool Is(const Block& block) const;

	const VoxelShape& GetShape() const;
	void UpdateNeighbourShapes(World& world, BlockPos pos, int flags) const;
	void UpdateIndirectNeighbourShapes(World& world, BlockPos pos, int flags) const;
	void NeighborChanged(World& world, BlockPos updatedPos, const Block& updaterBlock, BlockPos updaterPos) const;
	const BlockState& UpdateShape(Direction from, const BlockState& updaterState, World& world, BlockPos pos) const;
	void Tick(World& world, BlockPos pos) const;
	void RandomTick(World& world, BlockPos pos) const;
	bool CanBeReplaced(const BlockItemUseContext& useContext) const;
	bool Use(World& world, BlockPos pos, BlockRayTraceResult hitResult) const;
	bool IsFaceSturdy(const World& world, BlockPos pos, Direction dir) const;
	bool CanSurvive(const World& world, BlockPos pos) const;
	bool IsRedstoneConductor(const World& world, BlockPos pos) const;
	void OnPlace(World& world, BlockPos pos, const BlockState& oldState) const;
	void OnRemove(World& world, BlockPos pos, const BlockState& newState) const;
	int GetSignal(const World& world, BlockPos pos, Direction dir) const;
	int GetDirectSignal(const World& world, BlockPos pos, Direction dir) const;
	bool IsSignalSource() const;

	bool OccludesFace(Direction dir) const;
	bool OccludesAllFaces() const;
	RenderType GetRenderType() const;

	const BakedModel& GetModel() const;

private:
	uint16_t m_id;

	bool m_occludesFace[6];
	bool m_occludesAllFaces;
	RenderType m_renderType;
	const BakedModel* m_model;

	void FillCacheFields();

	friend class BlockRegistry;
	friend class BlockModelRegistry;
};
