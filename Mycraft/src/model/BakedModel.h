#pragma once

#include <functional>
#include <vector>

#include "../utils/Direction.h"
#include "../world/block/BlockPos.h"
#include "../world/block/BlockState.h"

struct BakedQuad {
	float data[54];
	int tintIx;

	static constexpr size_t vertex_size = 9; //3 pos, 2 uv, 1 tex, 3 color
	static constexpr size_t quad_size = 6 * vertex_size;
};

class BakedModel {

public:
	void WriteFace(std::vector<float>& target, const BlockState& state, BlockPos pos, Direction dir) const;
	bool HasFace(Direction dir) const;

private:
	std::vector<BakedQuad> m_quads[7];
	bool m_hasFace[7];
	std::function<glm::vec3(const BlockState& state, BlockPos pos, int ix)> m_colorFunc;

	void FillCacheFields();

	friend class UnbakedModel;
	friend class BlockColors;
	friend std::ostream& operator<<(std::ostream& os, const BakedModel& model);
};

