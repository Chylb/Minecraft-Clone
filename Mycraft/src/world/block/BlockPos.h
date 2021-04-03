#pragma once

struct BlockPos
{
	int x;
	int y;
	int z;

	BlockPos Top()
	{
		return BlockPos{ x,y + 1,z };
	}

	BlockPos North()
	{
		return BlockPos{ x,y,z - 1 };
	}

	BlockPos East()
	{
		return BlockPos{ x + 1,y,z };
	}

	BlockPos South()
	{
		return BlockPos{ x,y,z + 1 };
	}

	BlockPos West()
	{
		return BlockPos{ x - 1,y,z };
	}

	BlockPos Bottom()
	{
		return BlockPos{ x,y - 1,z };
	}
};