#include "Direction.h"

#include <algorithm>

Direction::Direction Direction::GetNearest(float x, float y, float z) {
	float max = std::max({ abs(x), abs(y), abs(z) });

	if (abs(x) == max)
		return x > 0 ? Direction::east : Direction::west;
	if (abs(y) == max)
		return y > 0 ? Direction::top : Direction::bottom;

	return z > 0 ? Direction::south : Direction::north;
}

Direction::Axis Direction::GetAxis(Direction dir)
{
	switch (dir) {
	case north:
		return Axis::z;
	case east:
		return Axis::x;
	case south:
		return Axis::z;
	case west:
		return Axis::x;
	case top:
		return Axis::y;
	case bottom:
		return Axis::y;
	}
}

std::ostream& Direction::operator<<(std::ostream& os, const Direction& dir)
{
	switch (dir) {
	case north:
		return os << "north";
	case east:
		return os << "east";
	case south:
		return os << "south";
	case west:
		return os << "west";
	case top:
		return os << "top";
	case bottom:
		return os << "bottom";
	case none:
		return os << "none";
	}
}
