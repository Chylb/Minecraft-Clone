#include "UnbakedQuad.h"

#include <glm/gtx/euler_angles.hpp>

UnbakedQuad::UnbakedQuad(glm::vec3 v1, glm::vec2 uv1, glm::vec3 v2, glm::vec2 uv2, glm::vec3 v3, glm::vec2 uv3, glm::vec3 v4, glm::vec2 uv4, std::string texture, Direction dir, int tint_ix)
{
	m_vertices[0] = glm::vec4(v1, 1.0);
	m_vertices[1] = glm::vec4(v2, 1.0);
	m_vertices[2] = glm::vec4(v3, 1.0);
	m_vertices[3] = glm::vec4(v4, 1.0);
	m_uvs[0] = uv1;
	m_uvs[1] = uv2;
	m_uvs[2] = uv3;
	m_uvs[3] = uv4;

	this->m_texture = texture;
	this->m_direction = dir;
	this->m_tint_ix = tint_ix;

	glm::vec3 vec1 = v2 - v1;
	glm::vec3 vec2 = v3 - v1;
	glm::vec3 cr = glm::cross(vec1, vec2);
	m_normal = glm::vec4(glm::cross(vec1, vec2), 1.0);
}

UnbakedQuad::UnbakedQuad(Direction dir, glm::vec4 uv, std::string texture, bool flip_y, bool clear_direction, int tint_ix)
{
	if (flip_y) //minecraft textures have flipped y axis 
	{
		auto y = uv.y;
		auto w = uv.w;
		uv.y = 16 - w;
		uv.w = 16 - y;
	}
	UnbakedQuad north{
		{1,0,0},{uv.x,uv.y},
		{0,0,0},{uv.z,uv.y},
		{0,1,0},{uv.z,uv.w},
		{1,1,0},{uv.x,uv.w},
		texture, Direction::north, tint_ix };
	north = north.Scale({ 16,16,16 });

	if (clear_direction)
		north.m_direction = Direction::none;

	switch (dir)
	{
	case Direction::down:
		*this = north.RotateX(-90).RotateY(180);
		return;
	case Direction::up:
		*this = north.RotateX(90).RotateY(180);
		return;
	case Direction::north:
		*this = north;
		return;
	case Direction::south:
		*this = north.RotateY(180);
		return;
	case Direction::west:
		*this = north.RotateY(90);
		return;
	case Direction::east:
		*this = north.RotateY(270);
		return;
	}
}

UnbakedQuad UnbakedQuad::Translate(glm::vec3 offset) {
	UnbakedQuad result(*this);
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = m_vertices[i] + glm::vec4(offset, 0);

	return result;
}

UnbakedQuad UnbakedQuad::Scale(glm::vec3 scale)
{
	UnbakedQuad result(*this);
	for (int i = 0; i < 4; i++) {
		result.m_vertices[i].x *= scale.x;
		result.m_vertices[i].y *= scale.y;
		result.m_vertices[i].z *= scale.z;
	}

	return result;
}

UnbakedQuad UnbakedQuad::RotateX(float ang, glm::vec3 origin) {
	UnbakedQuad result = Translate(-origin);
	auto matrix = glm::eulerAngleX(glm::radians(ang));
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = matrix * result.m_vertices[i];

	result.m_normal = matrix * m_normal;

	if (m_direction != Direction::none) {
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

UnbakedQuad UnbakedQuad::RotateY(float ang, glm::vec3 origin) {
	UnbakedQuad result = Translate(-origin);
	auto matrix = glm::eulerAngleY(glm::radians(ang));
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = matrix * result.m_vertices[i];

	result.m_normal = matrix * m_normal;

	if (m_direction != Direction::none) {
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

UnbakedQuad UnbakedQuad::RotateZ(float ang, glm::vec3 origin) {
	UnbakedQuad result = Translate(-origin);
	auto matrix = glm::eulerAngleZ(glm::radians(ang));
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = matrix * result.m_vertices[i];

	result.m_normal = matrix * m_normal;

	if (m_direction != Direction::none) {
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

UnbakedQuad UnbakedQuad::SetDirection(Direction dir)
{
	UnbakedQuad result = *this;
	result.m_direction = dir;
	return result;
}

BakedQuad UnbakedQuad::Bake()
{
	std::vector<float> buf;
	buf.reserve(BakedQuad::quad_size);

	auto& sprite = Resources::GetBlockTextureAtlas().GetSprite(m_texture.c_str());
	WriteVertex(buf, 0, sprite);
	WriteVertex(buf, 1, sprite);
	WriteVertex(buf, 2, sprite);
	WriteVertex(buf, 2, sprite);
	WriteVertex(buf, 3, sprite);
	WriteVertex(buf, 0, sprite);

	BakedQuad result{};
	std::memcpy(result.data, buf.data(), sizeof(float) * BakedQuad::quad_size);
	result.tintIx = m_tint_ix;
	return result;
}

void UnbakedQuad::WriteVertex(std::vector<float>& target, int vertex_ix, const TextureAtlasSprite& sprite) {
	target.push_back(m_vertices[vertex_ix].x / 16.0);
	target.push_back(m_vertices[vertex_ix].y / 16.0);
	target.push_back(m_vertices[vertex_ix].z / 16.0);
	target.push_back(sprite.uv0.x + (sprite.uv1.x - sprite.uv0.x) * m_uvs[vertex_ix].x / 16.0);
	target.push_back(sprite.uv0.y + (sprite.uv1.y - sprite.uv0.y) * m_uvs[vertex_ix].y / 16.0);
	target.push_back(1.0f);
	target.push_back(1.0f);
	target.push_back(1.0f);
}

std::ostream& operator<<(std::ostream& os, const UnbakedQuad& quad)
{
	os << "UnbakedQuad {" << quad.m_direction << "\n";
	for (auto v : quad.m_vertices) {
		os << v.x << " " << v.y << " " << v.z << "\n";
	}
	os << "}";
	return os;
}



