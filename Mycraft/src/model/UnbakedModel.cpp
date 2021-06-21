#include "UnbakedModel.h"

UnbakedModel::UnbakedModel(glm::vec3 from, glm::vec3 to, std::initializer_list<UnbakedQuad> quads) : m_quads(quads)
{
	auto size = (to - from) / 16.0f;
	*this = Scale(size).Translate(from);
}

void UnbakedModel::AddQuad(UnbakedQuad quad)
{
	m_quads.push_back(quad);
}

UnbakedModel UnbakedModel::AddModel(UnbakedModel model)
{
	UnbakedModel result = *this;

	for (auto& quad : model.m_quads)
		result.m_quads.push_back(quad);

	return result;
}

UnbakedModel& UnbakedModel::SetTexture(std::string oldTex, std::string newTex)
{
	for (auto& quad : m_quads)
		if (quad.m_texture == oldTex)
			quad.m_texture = newTex;

	return *this;
}

UnbakedModel UnbakedModel::Translate(glm::vec3 offset)
{
	UnbakedModel result = *this;
	for (auto& quad : result.m_quads)
		quad = quad.Translate(offset);
	return result;
}

UnbakedModel UnbakedModel::Scale(glm::vec3 scale)
{
	UnbakedModel result = *this;
	for (auto& quad : result.m_quads)
		quad = quad.Scale(scale);
	return result;
}

UnbakedModel UnbakedModel::RotateX(float ang, glm::vec3 origin)
{
	UnbakedModel result = *this;
	for (auto& quad : result.m_quads)
		quad = quad.RotateX(ang, origin);
	return result;
}

UnbakedModel UnbakedModel::RotateY(float ang, glm::vec3 origin, bool flip_none_directions)
{
	UnbakedModel result = *this;
	for (auto& quad : result.m_quads)
		quad = quad.RotateY(ang, origin);

	bool directionsToErase[6] = {};

	if (flip_none_directions) {
		for (auto& quad : result.m_quads)
			if (quad.m_direction == Direction::none)
			{
				Direction actualDirection = Direction::GetNearest(quad.m_normal.x, quad.m_normal.y, quad.m_normal.z);
				if (actualDirection.GetAxis() != Direction::Axis::y)
				{
					quad.m_direction = actualDirection;
					directionsToErase[actualDirection.GetOpposite()] = true;
				}
			}

		for (auto& quad : result.m_quads)
			if (directionsToErase[quad.m_direction])
				quad.m_direction = Direction::none;
	}

	return result;
}

UnbakedModel UnbakedModel::RotateZ(float ang, glm::vec3 origin)
{
	UnbakedModel result = *this;
	for (auto& quad : result.m_quads)
		quad = quad.RotateZ(ang, origin);
	return result;
}

BakedModel UnbakedModel::Bake()
{
	BakedModel model{};
	for (auto& quad : m_quads) {
		BakedQuad bakedQuad = quad.Bake();
		model.m_quads[quad.m_direction].push_back(bakedQuad);
	}
	model.FillCacheFields();

	return model;
}

std::ostream& operator<<(std::ostream& os, const UnbakedModel& model)
{
	os << "UnbakedModel {\n";
	for (auto& quad : model.m_quads)
		os << quad << "\n";
	os << "}";
	return os;
}
