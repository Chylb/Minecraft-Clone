#include "BlockState.h"

#include "Block.h"
#include "../world/World.h"
#include "../../item/BlockItemUseContext.h"
#include "../../utils/math/shapes/VoxelShapes.h"

uint16_t BlockState::GetId() const
{
	return m_id;
}

const Block& BlockState::GetBlock() const
{
	return *m_owner;
}

bool BlockState::Is(const Block& block) const
{
	return m_owner == &block;
}

const VoxelShape& BlockState::GetShape() const
{
	return m_owner->GetShape(*this);
}

void BlockState::UpdateNeighbourShapes(World& world, BlockPos pos) const
{
	for (Direction dir : Direction::directions)
	{
		auto neighborPos = pos.Adjacent(dir);
		auto currState = world.GetBlockState(neighborPos);
		auto& newState = currState->UpdateShape(dir.GetOpposite(), *this, world, neighborPos);

		if (&newState != currState)
			world.SetBlock(neighborPos, newState);
	}
}

const BlockState& BlockState::UpdateShape(Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	return m_owner->UpdateShape(*this, from, updaterState, world, pos);
}

void BlockState::Tick(World& world, BlockPos pos) const
{
	m_owner->Tick(world, pos);
}

bool BlockState::CanBeReplaced(const BlockItemUseContext& useContext) const
{
	return m_owner->CanBeReplaced(*this, useContext);
}

bool BlockState::Use(World& world, BlockPos pos, BlockRayTraceResult hitResult) const
{
	return m_owner->Use(*this, world, pos, hitResult);
}

bool BlockState::IsFaceSturdy(const World& world, BlockPos pos, Direction dir) const
{
	return &m_owner->GetShape(*this) == &VoxelShapes::Block();
}

bool BlockState::CanSurvive(const World& world, BlockPos pos) const
{
	return m_owner->CanSurvive(*this, world, pos);
}

bool BlockState::OccludesFace(Direction dir) const
{
	return m_occludesFace[dir];
}

bool BlockState::OccludesAllFaces() const
{
	return m_occludesAllFaces;
}

const BakedModel& BlockState::GetModel() const
{
	return *m_model;
}

void BlockState::FillCacheFields()
{
	bool occludesAll = true;
	for (Direction dir : Direction::directionsAndNone)
	{
		bool occludes = m_owner->OccludesFace(dir, *this);
		m_occludesFace[dir] = occludes;

		if (!occludes)
			occludesAll = false;
	}
	m_occludesAllFaces = occludesAll;
}