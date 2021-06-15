#include "BlockState.h"

#include "Block.h"
#include "../../item/BlockItemUseContext.h"

uint16_t BlockState::GetId() const
{
	return m_id;
}

const Block& BlockState::GetBlock() const
{
	return *m_owner;
}

const VoxelShape& BlockState::GetShape() const
{
	return m_owner->GetShape(*this);
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