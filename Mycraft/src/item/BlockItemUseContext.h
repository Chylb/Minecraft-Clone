#pragma once

#include "../world/block/BlockPos.h"
#include "../utils/math/BlockRayTraceResult.h"

class BlockState;
class Block;
class World;

class BlockItemUseContext
{
public:
	BlockItemUseContext(World* world, const BlockRayTraceResult& hitResult, const Block& placedBlock, glm::vec3 cameraDir);

	BlockPos GetClickedPos() const;
	Direction GetClickedFace() const;
	glm::vec3 GetClickLocation() const;
	const World& GetWorld() const;
	const Block& GetPlacedBlock() const;
	bool ReplacingClickedOnBlock() const;
	bool CanPlace() const;
	bool Place(const Block& block);

	Direction GetHorizontalDirection() const;
	std::array<Direction, 6> GetNearestLookingDirections() const;

private:
	glm::vec3 m_cameraDir;
	World* m_world;
	BlockRayTraceResult m_hitResult;
	BlockPos m_relativePos;
	const Block& m_placedBlock;
	bool m_replaceClicked = true;
};
