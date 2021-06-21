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
#include "../../utils/math/shapes/VoxelShapes.h"
#include "../../state/properties/BlockStateProperties.h"

class World;

class Block
{
public:
	Block(StateContainer<Block, BlockState>::Builder builder);
	Block();

	void CreateBlockStateDefinition(StateContainer<Block, BlockState>::Builder builder);
	void RegisterDefaultState(const BlockState& state);
	const StateContainer<Block, BlockState>& GetStateDefinition() const;
	const BlockState* DefaultBlockState() const;

	virtual const BlockState* GetStateForPlacement(const BlockItemUseContext& useContext) const;
	virtual const VoxelShape& GetShape(const BlockState& state) const;
	virtual const BlockState& UpdateShape(const BlockState& oldState, Direction from, const BlockState& updaterState, World& world, BlockPos pos) const;
	virtual void Tick(World& world, BlockPos pos) const;
	virtual bool CanBeReplaced(const BlockState& state, const BlockItemUseContext& useContext) const;
	virtual bool Use(const BlockState& state, World& world, BlockPos pos, BlockRayTraceResult hitResult) const;
	virtual bool OccludesFace(Direction dir, const BlockState& state) const;
	virtual bool CanSurvive(const BlockState& state, const World& world, BlockPos pos) const;
	virtual void SetPlacedBy(World& world, BlockPos pos, const BlockState& state) const;

	static VoxelShape Box(float x0, float y0, float z0, float x1, float y1, float z1);

protected:
	StateContainer<Block, BlockState> m_stateDefinition;
	const BlockState* m_defaultBlockState;
	bool m_opaque;

	friend class BlockRegistry;
};
