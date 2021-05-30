#include "UnbakedQuad.h"

#include <glm/gtx/euler_angles.hpp>

#include "../Resources.h"

UnbakedQuad::UnbakedQuad(glm::vec3 v1, glm::vec2 uv1, glm::vec3 v2, glm::vec2 uv2, glm::vec3 v3, glm::vec2 uv3, glm::vec3 v4, glm::vec2 uv4, std::string texture, Direction::Direction dir)
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

	if (m_direction != Direction::none) {
		glm::vec3 vec1 = v2 - v1;
		glm::vec3 vec2 = v3 - v1;
		glm::vec3 cr = glm::cross(vec1, vec2);
		m_normal = glm::vec4(glm::cross(vec1, vec2), 1.0);
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

	if (m_direction != Direction::none) {
		result.m_normal = matrix * m_normal;
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

UnbakedQuad UnbakedQuad::RotateY(float ang, glm::vec3 origin) {
	UnbakedQuad result = Translate(-origin);
	auto matrix = glm::eulerAngleY(glm::radians(ang));
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = matrix * result.m_vertices[i];

	if (m_direction != Direction::none) {
		result.m_normal = matrix * m_normal;
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

UnbakedQuad UnbakedQuad::RotateZ(float ang, glm::vec3 origin) {
	UnbakedQuad result = Translate(-origin);
	auto matrix = glm::eulerAngleZ(glm::radians(ang));
	for (int i = 0; i < 4; i++)
		result.m_vertices[i] = matrix * result.m_vertices[i];

	if (m_direction != Direction::none) {
		result.m_normal = matrix * m_normal;
		result.m_direction = Direction::GetNearest(result.m_normal.x, result.m_normal.y, result.m_normal.z);
	}

	return result.Translate(origin);
}

BakedQuad UnbakedQuad::Bake()
{
	int texture_ix = Resources::GetTextureIx(m_texture);

	std::vector<float> buf;
	buf.reserve(36);
	WriteVertex(buf, 0, texture_ix);
	WriteVertex(buf, 1, texture_ix);
	WriteVertex(buf, 2, texture_ix);
	WriteVertex(buf, 2, texture_ix);
	WriteVertex(buf, 3, texture_ix);
	WriteVertex(buf, 0, texture_ix);

	BakedQuad result{};
	std::memcpy(result.data, buf.data(), sizeof(float) * 36);
	return result;
}

void UnbakedQuad::WriteVertex(std::vector<float>& target, int vertex_ix, int texture_ix) {
	target.push_back(m_vertices[vertex_ix].x / 16.0);
	target.push_back(m_vertices[vertex_ix].y / 16.0);
	target.push_back(m_vertices[vertex_ix].z / 16.0);
	target.push_back(m_uvs[vertex_ix].x);
	target.push_back(m_uvs[vertex_ix].y);
	target.push_back(texture_ix);
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
