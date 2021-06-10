#pragma once

#include <vector>

#include "../utils/Direction.h"
#include "../world/block/BlockPos.h"

struct BakedQuad {
	float data[36];
};

class BakedModel {

public:
	void WriteFace(std::vector<float>& target, BlockPos pos, Direction dir = Direction::none) const;
	bool HasFace(Direction dir) const;

private:
	std::vector<BakedQuad> m_quads[7];
	bool m_hasFace[7];

	void FillCacheFields();

	friend class UnbakedModel;
	friend std::ostream& operator<<(std::ostream& os, const BakedModel& model);
};

