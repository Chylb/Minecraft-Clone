#include "VoxelShape.h"

#include <iostream>

VoxelShape::VoxelShape(std::initializer_list<AxisAlignedBB> aabbs) : m_aabbs(aabbs)
{
}

BlockRayTraceResult VoxelShape::Clip(glm::vec3 from, glm::vec3 rayDir, BlockPos pos) const
{
	Direction direction;
	float t_min = INFINITY;

	for (auto& aabb : m_aabbs)
	{
		auto [dir, t] = aabb.GetHitDirection(from, rayDir, pos);
		if (t < t_min)
		{
			t_min = t;
			direction = dir;
		}
	}
	if (t_min == NAN || t_min > 1)
		return { .hit = false };

	glm::vec3 location = from + rayDir * t_min;
	return {location, true, pos, direction };
}

std::ostream& operator<<(std::ostream& os, const VoxelShape& shape)
{
	os << "Shape {\n";
	for (auto& aabb : shape.m_aabbs)
	{
		os << aabb;
	}
	os << "}";
	return os;
}
