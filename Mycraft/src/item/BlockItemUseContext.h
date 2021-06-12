#pragma once

#include "../world/block/BlockPos.h"
#include "../utils/math/BlockRayTraceResult.h"

class BlockState;
class Block;
class World;

class BlockItemUseContext
{
public:
	BlockItemUseContext(World* world, const BlockRayTraceResult& hitResult, const Block& placedBlock);

	BlockPos GetClickedPos() const;
	Direction GetClickedFace() const;
	glm::vec3 GetClickLocation() const;
	const World& GetWorld() const;
	const Block& GetPlacedBlock() const;
	bool ReplacingClickedOnBlock() const;
	bool CanPlace() const;
	bool Place(const Block& block);

private:
	World* m_world;
	BlockRayTraceResult m_hitResult;
	BlockPos m_relativePos;
	const Block& m_placedBlock;
	bool m_replaceClicked = true;
};
