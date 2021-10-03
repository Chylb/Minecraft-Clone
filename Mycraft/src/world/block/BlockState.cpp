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

void BlockState::UpdateNeighbourShapes(World& world, BlockPos pos, int flags) const
{
	for (Direction dir : Direction::directions)
	{
		auto neighborPos = pos.Adjacent(dir);
		auto& oldState = *world.GetBlockState(neighborPos);
		auto& newState = oldState.UpdateShape(dir.GetOpposite(), *this, world, neighborPos);

		Block::UpdateOrDestroy(oldState, newState, world, neighborPos, flags);
	}
}

void BlockState::UpdateIndirectNeighbourShapes(World& world, BlockPos pos, int flags) const
{
	m_owner->UpdateIndirectNeighbourShapes(*this, world, pos, flags);
}

void BlockState::NeighborChanged(World& world, BlockPos updatedPos, const Block& updaterBlock, BlockPos updaterPos) const
{
	m_owner->NeighborChanged(*this, world, updatedPos, updaterBlock, updaterPos);
}

const BlockState& BlockState::UpdateShape(Direction from, const BlockState& updaterState, World& world, BlockPos pos) const
{
	return m_owner->UpdateShape(*this, from, updaterState, world, pos);
}

void BlockState::Tick(World& world, BlockPos pos) const
{
	m_owner->Tick(*this, world, pos);
}

void BlockState::RandomTick(World& world, BlockPos pos) const
{
	m_owner->RandomTick(*this, world, pos);
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

bool BlockState::IsRedstoneConductor(const World& world, BlockPos pos) const
{
	return &GetShape() == &VoxelShapes::Block();
}

void BlockState::OnPlace(World& world, BlockPos pos, const BlockState& oldState) const
{
	m_owner->OnPlace(*this, world, pos, oldState);
}

void BlockState::OnRemove(World& world, BlockPos pos, const BlockState& newState) const
{
	m_owner->OnRemove(*this, world, pos, newState);
}

int BlockState::GetSignal(const World& world, BlockPos pos, Direction dir) const
{
	return m_owner->GetSignal(*this, world, pos, dir);
}

int BlockState::GetDirectSignal(const World& world, BlockPos pos, Direction dir) const
{
	return m_owner->GetDirectSignal(*this, world, pos, dir);
}

bool BlockState::IsSignalSource() const
{
	return m_owner->IsSignalSource(*this);
}

bool BlockState::OccludesFace(Direction dir) const
{
	return m_occludesFace[dir];
}

bool BlockState::OccludesAllFaces() const
{
	return m_occludesAllFaces;
}

RenderType BlockState::GetRenderType() const
{
	return m_renderType;
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

	m_renderType = m_owner->GetRenderType(*this);
}