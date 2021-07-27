#include "BlockItemUseContext.h"

#include "../world/World.h"

BlockItemUseContext::BlockItemUseContext(World* world, const BlockRayTraceResult& hitResult, const Block& placedBlock, glm::vec3 cameraDir) :
	m_world(world),
	m_hitResult(hitResult),
	m_relativePos(hitResult.blockPos.Adjacent(hitResult.direction)),
	m_placedBlock(placedBlock),
	m_cameraDir(cameraDir)
{
	m_replaceClicked = world->GetBlockState(hitResult.blockPos)->CanBeReplaced(*this);
}

BlockPos BlockItemUseContext::GetClickedPos() const
{
	return m_replaceClicked ? m_hitResult.blockPos : m_relativePos;
}

Direction BlockItemUseContext::GetClickedFace() const
{
	return m_hitResult.direction;
}

glm::vec3 BlockItemUseContext::GetClickLocation() const
{
	return m_hitResult.location;
}

const World& BlockItemUseContext::GetWorld() const
{
	return *m_world;
}

const Block& BlockItemUseContext::GetPlacedBlock() const
{
	return m_placedBlock;
}

bool BlockItemUseContext::ReplacingClickedOnBlock() const
{
	return m_replaceClicked;
}

bool BlockItemUseContext::CanPlace() const
{
	return m_replaceClicked || m_world->GetBlockState(GetClickedPos())->CanBeReplaced(*this);
}

bool BlockItemUseContext::Place(const Block& block)
{
	if (!CanPlace())
		return false;

	auto state = block.GetStateForPlacement(*this);
	if (state != nullptr && state->CanSurvive(*m_world, GetClickedPos())) {
		m_world->SetBlock(GetClickedPos(), *state, 11);
		block.SetPlacedBy(*m_world, GetClickedPos(), *state);
		return true;
	}
	return false;
}

Direction BlockItemUseContext::GetHorizontalDirection() const
{
	return Direction::GetNearest(m_cameraDir.x, 0, m_cameraDir.z);
}

std::array<Direction, 6> BlockItemUseContext::GetNearestLookingDirections() const
{
	auto orderedDirections = Direction::OrderedByNearest(m_cameraDir);
	if (m_replaceClicked)
		return orderedDirections;

	auto clickedFace = GetClickedFace();

	int i;
	for (i = 0; i < orderedDirections.size() && orderedDirections[i] != clickedFace.GetOpposite(); i++) {}

	if (i > 0)
	{
		for (int j = 0; j < i; j++)
			orderedDirections[j + 1] = orderedDirections[j];
		orderedDirections[0] = clickedFace.GetOpposite();
	}
	return orderedDirections;
}
