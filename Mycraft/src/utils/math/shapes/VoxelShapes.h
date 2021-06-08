#pragma once
#include "VoxelShape.h"

namespace VoxelShapes
{
	namespace
	{
		const VoxelShape block{ {0,0,0,1,1,1} };
		const VoxelShape empty{};
	}
	
	const VoxelShape& Block();
	const VoxelShape& Empty();
};
