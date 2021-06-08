#pragma once

#include <vector>

#include "../BlockRayTraceResult.h"
#include "../AxisAlignedBB.h"
#include "glm/vec3.hpp"

class VoxelShape
{
public:
	VoxelShape(std::initializer_list<AxisAlignedBB> aabbs);

	BlockRayTraceResult Clip(glm::vec3 from, glm::vec3 rayDir, BlockPos pos) const;

private:
	std::vector<AxisAlignedBB> m_aabbs;

	friend std::ostream& operator<<(std::ostream& os, const VoxelShape& shape);
};
