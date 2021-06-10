#pragma once

#include "../../state/StateHolder.h"

#include "../../utils/Direction.h"

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
	bool CanBeReplaced(BlockRayTraceResult rayTraceResult) const;

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
