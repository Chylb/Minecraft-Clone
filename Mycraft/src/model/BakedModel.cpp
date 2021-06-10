#include "BakedModel.h"

void BakedModel::WriteFace(std::vector<float>& target, BlockPos pos, Direction dir) const
{
	for (auto face : m_quads[dir]) {
		for (int i = 0; i < 36; i += 6) {
			face.data[i] += pos.x;
		}
		for (int i = 1; i < 36; i += 6) {
			face.data[i] += pos.y;
		}
		for (int i = 2; i < 36; i += 6) {
			face.data[i] += pos.z;
		}
		target.insert(target.end(), face.data, face.data + 36);
	}
}

bool BakedModel::HasFace(Direction dir) const
{
	return m_hasFace[dir];
}

void BakedModel::FillCacheFields()
{
	for (int i = 0; i < 7; i++)
		if (m_quads[i].size())
			m_hasFace[i] = true;
}

std::ostream& operator<<(std::ostream& os, const BakedModel& model)
{
	os << "BakedModel {" << "\n";
	for (int dir = 0; dir < 7; dir++) {
		os << "dir " << dir << "\n";
		for (auto& quad : model.m_quads[dir]) {
			for (auto num : quad.data) {
				os << num << " ";
			}
			os << "\n";
		}
	}
	os << "}";
	return os;
}
