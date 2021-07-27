#pragma once

#include "block/BlockPos.h"

template<typename T>
struct NextTickListEntry
{
	BlockPos pos;
	const T& type;
	uint64_t triggerTick;

	NextTickListEntry(BlockPos _pos, const T& _type, uint64_t _triggerTick = 0) :
		pos(_pos),
		type(_type),
		triggerTick(_triggerTick)
	{
	}

	NextTickListEntry& operator=(const NextTickListEntry& other)
	{
		new (this) NextTickListEntry(other);
		return *this;
	}

	bool operator==(const NextTickListEntry& other) const
	{
		return pos == other.pos && &type == &other.type;
	}

	struct CompareTick
	{
		bool operator()(NextTickListEntry const& e1, NextTickListEntry const& e2)
		{
			return e1.triggerTick > e2.triggerTick;
		}
	};

	struct HashFunction
	{
		std::size_t operator()(const NextTickListEntry& k) const
		{
			std::size_t posHash = std::hash<uint64_t>()(k.pos.x);
			return posHash;
		}
	};
};