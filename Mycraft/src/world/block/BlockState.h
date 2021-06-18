#pragma once

#include "../../state/StateHolder.h"

#include "../../utils/Direction.h"

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

	uint16_t GetId() const;
	const Block& GetBlock() const;

	const VoxelShape& GetShape() const;
	void Tick(World& world, BlockPos pos) const;
	bool CanBeReplaced(const BlockItemUseContext& useContext) const;
	bool Use(World& world, BlockPos pos, BlockRayTraceResult hitResult) const;
	bool IsFaceSturdy(const World& world, BlockPos pos, Direction dir) const;
	bool CanSurvive(const World& world, BlockPos pos) const;

	bool OccludesFace(Direction dir) const;
	bool OccludesAllFaces() const;

	const BakedModel& GetModel() const;

private:
	uint16_t m_id;

	bool m_occludesFace[6];
	bool m_occludesAllFaces;
	const BakedModel* m_model;

	void FillCacheFields();

	friend class BlockRegistry;
	friend class BlockModelRegistry;
};
