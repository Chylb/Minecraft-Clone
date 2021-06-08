#include "AxisAlignedBB.h"

#include <algorithm>

std::pair<Direction::Direction, float> AxisAlignedBB::GetHitDirection(glm::vec3 from, glm::vec3 rayDir, BlockPos pos) const
{
	float t1 = (minX + pos.x - from.x) / rayDir.x;
	float t2 = (maxX + pos.x - from.x) / rayDir.x;
	float t3 = (minY + pos.y - from.y) / rayDir.y;
	float t4 = (maxY + pos.y - from.y) / rayDir.y;
	float t5 = (minZ + pos.z - from.z) / rayDir.z;
	float t6 = (maxZ + pos.z - from.z) / rayDir.z;

	//float tmin = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
	//float tmax = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });
	float tmin = -1;
	float tmax = 1;
	float new_tmin, new_tmax;
	Direction::Direction dir, new_dir;

	if (t1 < t2)
	{
		new_tmin = t1;
		new_tmax = t2;
		new_dir = Direction::west;
	}
	else
	{
		new_tmin = t2;
		new_tmax = t1;
		new_dir = Direction::east;
	}

	if (new_tmin > tmin) {
		tmin = new_tmin;
		dir = new_dir;
	}
	if (new_tmax < tmax)
		tmax = new_tmax;

	if (t3 < t4)
	{
		new_tmin = t3;
		new_tmax = t4;
		new_dir = Direction::bottom;
	}
	else
	{
		new_tmin = t4;
		new_tmax = t3;
		new_dir = Direction::top;
	}

	if (new_tmin > tmin) {
		tmin = new_tmin;
		dir = new_dir;
	}
	if (new_tmax < tmax)
		tmax = new_tmax;

	if (t5 < t6)
	{
		new_tmin = t5;
		new_tmax = t6;
		new_dir = Direction::north;
	}
	else
	{
		new_tmin = t6;
		new_tmax = t5;
		new_dir = Direction::south;
	}

	if (new_tmin > tmin) {
		tmin = new_tmin;
		dir = new_dir;
	}
	if (new_tmax < tmax)
		tmax = new_tmax;

	if (tmax < 0) {
		return { Direction::none,NAN };
	}

	if (tmin > tmax) {
		return { Direction::none,NAN };
	}

	if (tmin > 1)
		return { Direction::none,NAN };

	return { dir, tmin };
}

std::ostream& operator<<(std::ostream& os, const AxisAlignedBB& aabb)
{
	os << "AABB { " << aabb.minX << " " << aabb.minY << " " << aabb.minZ << " " << aabb.maxX << " " << aabb.maxY << " " << aabb.maxZ << " }\n";
	return os;
}
