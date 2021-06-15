#pragma once

#include <GL/glew.h>

#include <vector>
#include <unordered_map>

#include "BlockPos.h"
#include "BlockState.h"
#include "../../state/Property.h"
#include "../state/StateContainer.h"
#include "../../utils/math/BlockRayTraceResult.h"
#include "../../utils/math/shapes/VoxelShape.h"
#include "../../state/properties/BlockStateProperties.h"

class World;

class Block
{
public:
	Block(StateContainer<Block, BlockState>::Builder builder);
	Block();

	const StateContainer<Block, BlockState>& GetStateDefinition() const;
	BlockState* DefaultBlockState() const;

	virtual const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const;
	virtual const VoxelShape& GetShape(const BlockState& state) const;
	virtual void Tick(World& world, BlockPos pos) const;
	virtual bool CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const;
	virtual bool Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const;
	virtual bool OccludesFace(Direction dir, const BlockState& state) const;

	static VoxelShape Box(float x0, float y0, float z0, float x1, float y1, float z1);

protected:
	StateContainer<Block, BlockState> m_stateDefinition;
	BlockState* m_defaultBlockState;
	bool m_opaque;

	friend class BlockRegistry;
};
