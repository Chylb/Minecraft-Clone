#pragma once

#include <GL/glew.h>

#include <vector>
#include <unordered_map>

#include "BlockPos.h"
#include "BlockState.h"
#include "../../state/Property.h"
#include "../state/StateContainer.h"
#include "../../utils/math/BlockRayTraceResult.h"

struct VoxelShape;
class World;

class Block
{
public:
	Block(bool opaque, StateContainer<Block, BlockState>::Builder builder);
	Block(bool opaque);

	bool IsOpaque() const;

	const StateContainer<Block, BlockState>& GetStateDefinition() const;
	BlockState* DefaultBlockState() const;

	virtual const BlockState* GetStateForPlacement(BlockRayTraceResult rayTraceResult) const;

	virtual const VoxelShape& GetShape(const BlockState& state) const;
	virtual void Tick(World& world, BlockPos pos) const;
	virtual bool CanBeReplaced(const BlockState& state, BlockRayTraceResult rayTraceResult) const;
	virtual bool OccludesFace(Direction dir, const BlockState& state) const;

protected:
	StateContainer<Block, BlockState> m_stateDefinition;
	BlockState* m_defaultBlockState;
	bool m_opaque;

	friend class BlockRegistry;
};
