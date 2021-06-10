#pragma once
#include <glm/glm.hpp>

#include "BakedModel.h"
#include "../utils/Direction.h"

struct UnbakedQuad {
	UnbakedQuad(glm::vec3 v1, glm::vec2 uv1, glm::vec3 v2, glm::vec2 uv2, glm::vec3 v3, glm::vec2 uv3, glm::vec3 v4, glm::vec2 uv4,
		std::string texture, Direction dir = Direction::none);

	UnbakedQuad(Direction dir, glm::vec4 uv, std::string texture, bool flip_y = true, bool clear_direction = false);

	UnbakedQuad Translate(glm::vec3 offset);
	UnbakedQuad Scale(glm::vec3 scale);
	UnbakedQuad RotateX(float ang, glm::vec3 origin = { 8,8,8 });
	UnbakedQuad RotateY(float ang, glm::vec3 origin = { 8,8,8 });
	UnbakedQuad RotateZ(float ang, glm::vec3 origin = { 8,8,8 });
	UnbakedQuad SetDirection(Direction dir);

	BakedQuad Bake();

private:
	std::string m_texture;
	Direction m_direction;
	glm::vec4 m_normal;
	glm::vec4 m_vertices[4];
	glm::vec2 m_uvs[4];

	void WriteVertex(std::vector<float>& target, int vertex_ix, int texture_ix);

	friend class UnbakedModel;
	friend std::ostream& operator<<(std::ostream& os, const UnbakedQuad& quad);
};