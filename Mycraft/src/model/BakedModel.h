#pragma once

#include <vector>

#include "../Direction.h"
#include "../world/block/BlockPos.h"

struct BakedQuad {
	float data[36];
};

class BakedModel {

public:
	void WriteFace(std::vector<float>& target, BlockPos pos, Direction::Direction dir = Direction::none) const;

private:
	std::vector<BakedQuad> m_quads[7];

	friend class UnbakedModel;
	friend std::ostream& operator<<(std::ostream& os, const BakedModel& model);
};

