#pragma once

#include <ostream>

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
	enum Value : uint8_t
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

	constexpr  Direction(Value value = north) : _value(value) { }
	constexpr operator int() const { return _value; }

	constexpr Direction GetOpposite() const;
	constexpr Axis GetAxis() const;
	constexpr bool IsHorizontal() const;
	static Direction GetNearest(float x, float y, float z);

	static constexpr Value directions[]{ north, east, south, west, up, down };
	static constexpr Value directionsAndNone[]{ north, east, south, west, up, down, none };

	Value _value;
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