#pragma once

#include <ostream>
#include <array>

#include "glm/vec3.hpp"

#define FOREACH_HORIZONTAL_DIRECTION(VAR, CODE) \
	{VAR = Direction::north; do CODE while(0);} \
	{VAR = Direction::east; do CODE while(0);} \
	{VAR = Direction::south; do CODE while(0);} \
	{VAR = Direction::west; do CODE while(0);}

#define FOREACH_DIRECTION(VAR, CODE) \
	FOREACH_HORIZONTAL_DIRECTION(VAR, CODE) \
	{VAR = Direction::up; do CODE while(0);} \
	{VAR = Direction::down; do CODE while(0);}

class Direction
{
public:
	enum Value
	{
		north,
		east,
		south,
		west,
		up,
		down,
		none
	};

	enum class Axis
	{
		x, y, z
	};

	constexpr  Direction(Value value = north) : _value(value) {}
	explicit constexpr Direction(int direction) : _value((Value)direction) {}
	constexpr operator int() const { return _value; }

	constexpr Direction GetOpposite() const;
	constexpr Direction GetClockWise() const;
	constexpr Direction GetCounterClockWise() const;
	constexpr glm::vec3 GetVector() const;
	constexpr Axis GetAxis() const;
	constexpr bool IsHorizontal() const;
	static Direction GetNearest(float x, float y, float z);
	static std::array<Direction, 6> OrderedByNearest(glm::vec3);

	static constexpr Value directions[]{ north, east, south, west, up, down };
	static constexpr Value directionsAndNone[]{ north, east, south, west, up, down, none };

	Value _value;

private:
	static std::array<Direction, 6> MakeDirectionArray(Direction dir1, Direction dir2, Direction dir3);
};

constexpr Direction Direction::GetOpposite() const
{
	switch (_value) {
	case north:
		return south;
	case east:
		return west;
	case south:
		return north;
	case west:
		return east;
	case up:
		return down;
	case down:
		return up;
	case none:
		return none;
	}
}

constexpr Direction Direction::GetClockWise() const
{
	switch (_value) {
	case north:
		return east;
	case south:
		return west;
	case west:
		return north;
	case east:
		return south;
	}
}

constexpr Direction Direction::GetCounterClockWise() const
{
	switch (_value) {
	case north:
		return west;
	case south:
		return east;
	case west:
		return south;
	case east:
		return north;
	}
}

constexpr glm::vec3 Direction::GetVector() const
{
	switch (_value) {
	case north:
		return { 0,0,-1 };
	case east:
		return { 1,0,0 };
	case south:
		return { 0,0,1 };
	case west:
		return { -1,0,0 };
	case up:
		return { 0,1,0 };
	case down:
		return { 0,-1,0 };
	}
}

constexpr Direction::Axis Direction::GetAxis() const
{
	switch (_value) {
	case north:
		return Axis::z;
	case east:
		return Axis::x;
	case south:
		return Axis::z;
	case west:
		return Axis::x;
	case up:
		return Axis::y;
	case down:
		return Axis::y;
	}
}

constexpr bool Direction::IsHorizontal() const
{
	return !(_value == up || _value == down);
}

inline Direction Direction::GetNearest(float x, float y, float z)
{
	float max = std::max({ abs(x), abs(y), abs(z) });

	if (abs(x) == max)
		return x > 0 ? east : west;
	if (abs(y) == max)
		return y > 0 ? up : down;

	return z > 0 ? south : north;
}

inline std::array<Direction, 6> Direction::OrderedByNearest(glm::vec3 vec)
{
	Direction directions[3];
	directions[0] = vec[0] >= 0 ? east : west;
	directions[1] = vec[1] >= 0 ? up : down;
	directions[2] = vec[2] >= 0 ? south : north;

	int order[] = { 0,1,2 };
	for (int i = 0; i < 3; i++)
		for (int j = i + 1; j < 3; j++)
			if (abs(vec[order[i]]) < abs(vec[order[j]]))
				std::swap(order[i], order[j]);

	return MakeDirectionArray(directions[order[0]], directions[order[1]], directions[order[2]]);
}

inline std::array<Direction, 6> Direction::MakeDirectionArray(Direction dir1, Direction dir2, Direction dir3)
{
	return { dir1, dir2, dir3, dir3.GetOpposite(), dir2.GetOpposite(), dir1.GetOpposite() };
}

